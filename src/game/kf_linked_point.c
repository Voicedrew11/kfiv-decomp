#include "kf_linked_point.h"

void *memset(void *, int, u32);
void Vector_Copy(void *, void *);
void Vector_Add(void *, void *, void *);
void sceVu0SubVector(void *, void *, void *);
float sqrtf(float);

static LinkedPoint *linked_point_heads[2]; /* 0x4470C8 two active lists */
static LinkedPoint *linked_point_free;     /* 0x4470D0 free list */

static LinkedPoint *LinkedPoint_TakeFree(void);
static void LinkedPoint_PushFree(LinkedPoint *point);
static LinkedPoint *LinkedPoint_UnlinkActive(LinkedPoint *point);
static void LinkedPoint_LinkActive(LinkedPoint *point, u32 list);

/* Clears the pool, chains every entry on the free list and empties the two active lists. */
void LinkedPoint_Init(void) {
    LinkedPoint *node;
    int i;

    memset(linked_point_pool, 0, sizeof(linked_point_pool));
    node = linked_point_pool;
    node->handle = 0;
    node->prev = NULL;
    node->next = node + 1;
    node++;
    for (i = 1; i < LINKED_POINT_COUNT - 1; i++) {
        node->handle = i;
        node->prev = node - 1;
        node->next = node + 1;
        node++;
    }
    node->handle = LINKED_POINT_COUNT - 1;
    node->prev = node - 1;
    node->next = NULL;
    for (i = 1; i >= 0; i--) {
        linked_point_heads[i] = NULL;
    }
    linked_point_free = linked_point_pool;
}

/* Starts a point with its owner slot fixed at -1; returns the pool index or -1. */
int LinkedPoint_Start(f32 duration, void *start, void *end, int kind_a, int kind_b, u32 list) {
    return LinkedPoint_Allocate(duration, start, end, kind_a, kind_b, -1, list);
}

/* Takes a node from the pool, links it into list `list` and fills it in; returns its id or -1. */
int LinkedPoint_Allocate(f32 duration, void *start, void *end, u8 kind_a, int kind_b, int owner, u32 list) {
    LinkedPoint *point;

    if (list >= 2) {
        return -1;
    }
    point = LinkedPoint_TakeFree();
    if (point == NULL) {
        return -1;
    }
    LinkedPoint_LinkActive(point, list);
    Vector_Copy(point, start);
    point->follow = start;
    Vector_Copy(&point->target, end);
    point->kind_a = kind_a;
    point->kind_b = kind_b;
    point->unk32 = owner;
    point->target.w = 1.0f;
    point->duration = duration;
    point->position.w = 1.0f / duration;
    point->progress_scale = 1.0f / (duration * duration);
    return point->handle;
}

int LinkedPoint_StartDefault(f32 duration, void *start, void *end) {
    return LinkedPoint_Allocate(duration, start, end, 0, -1, -1, 0);
}

/* Unlinks and frees the pool entry at index. */
void LinkedPoint_ReleaseIndex(u32 index) {
    LinkedPoint *node;

    if (index < LINKED_POINT_COUNT) {
        node = &linked_point_pool[index];
        LinkedPoint_UnlinkActive(node);
        LinkedPoint_PushFree(node);
    }
}

/* Releases every active point of the given kind from both active lists. */
void LinkedPoint_ReleaseByKind(int kind) {
    int i;
    LinkedPoint *node;
    LinkedPoint *next;

    for (i = 0; i < 2; i++) {
        node = linked_point_heads[i];
        while (node != NULL) {
            if (node->active != 0 && node->kind_b == kind) {
                next = LinkedPoint_UnlinkActive(node);
                LinkedPoint_PushFree(node);
                node = next;
            } else {
                node = node->next;
            }
        }
    }
}

/* Collects the handles of the linked points whose reach touches the model (at most 4). */
void LinkedPoint_FindHitsForModel(ModelInstance *model) {
    int i;
    LinkedPoint *node;
    f32 delta[4];
    f32 dist;

    model->hit_count = 0;
    for (i = 1; i >= 0; i--) {
        for (node = linked_point_heads[i]; node != NULL; node = node->next) {
            sceVu0SubVector(delta, &model->position, node);
            dist = sqrtf(delta[0] * delta[0] + delta[1] * delta[1] + delta[2] * delta[2]);
            if (dist - model->radius <= node->duration) {
                model->hit_handles[model->hit_count++] = node->handle;
                if (model->hit_count >= 4) {
                    return;
                }
            }
        }
    }
}

/* Collects handles of linked points (except one) whose reach touches the model (at most 4). */
void LinkedPoint_FindHitsExclude(ModelInstance *model, int handle) {
    int i;
    LinkedPoint *node;
    f32 delta[4];
    f32 dist;

    model->hit_count = 0;
    for (i = 1; i >= 0; i--) {
        for (node = linked_point_heads[i]; node != NULL; node = node->next) {
            if (node->handle == handle) {
                continue;
            }
            sceVu0SubVector(delta, &model->position, node);
            dist = sqrtf(delta[0] * delta[0] + delta[1] * delta[1] + delta[2] * delta[2]);
            if (dist - model->radius <= node->duration) {
                model->hit_handles[model->hit_count++] = node->handle;
                if (model->hit_count >= 4) {
                    return;
                }
            }
        }
    }
}

/* Collects handles for one owner (or any owner) whose reach touches the model (at most 4). */
void LinkedPoint_FindHitsForOwner(ModelInstance *model, int owner) {
    int i;
    LinkedPoint *node;
    f32 delta[4];
    f32 dist;

    model->hit_count = 0;
    for (i = 1; i >= 0; i--) {
        for (node = linked_point_heads[i]; node != NULL; node = node->next) {
            if (node->unk32 != -1 && node->unk32 != owner) {
                continue;
            }
            sceVu0SubVector(delta, &model->position, node);
            dist = sqrtf(delta[0] * delta[0] + delta[1] * delta[1] + delta[2] * delta[2]);
            if (dist - model->radius <= node->duration) {
                model->hit_handles[model->hit_count++] = node->handle;
                if (model->hit_count >= 4) {
                    return;
                }
            }
        }
    }
}

/* Collects handles whose reach touches the map record's box (at most 4, at most 2 per list). */
void LinkedPoint_FindHitsForMapDraw(MapDrawRecord *record, ModelInstance *model) {
    f32 unused[4]; // match: the frame has 16 unused bytes below the counters
    s32 taken[2];
    s32 *taken_start = taken; // match: memset through a pointer is a call; on the array itself it is inlined
    int i;
    LinkedPoint *node;
    f32 reach;

    memset(taken_start, 0, sizeof(taken));
    model->hit_count = 0;
    for (i = 1; i >= 0; i--) {
        for (node = linked_point_heads[i]; node != NULL; node = node->next) {
            if (taken[node->list_index] >= 2) {
                continue;
            }
            if (node->unk32 != -1 && node->unk32 != record->unk38) {
                continue;
            }
            reach = node->duration;
            if (!(0.0f < reach)) {
                continue;
            }
            if (record->unk40[0] - reach <= node->position.x && node->position.x <= record->unk50[0] + reach &&
                record->unk40[1] - reach <= node->position.y && node->position.y <= record->unk50[1] + reach &&
                record->unk40[2] - reach <= node->position.z && node->position.z <= record->unk50[2] + reach) {
                model->hit_handles[model->hit_count++] = node->handle;
                taken[node->list_index]++;
                if (model->hit_count >= 4) {
                    return;
                }
            }
        }
    }
}

void LinkedPoint_UpdateFollows(void) {
    int i;
    LinkedPoint *node;

    for (i = 0; i < 2; i++) {
        for (node = linked_point_heads[i]; node != NULL; node = node->next) {
            if (node->follow != NULL) {
                f32 saved = node->position.w;

                Vector_Copy(node, node->follow);
                node->position.w = saved;
            }
        }
    }
}

void LinkedPoint_TransformFollows(void *transform) {
    int i;
    LinkedPoint *node;

    i = 0;
    do {
        node = linked_point_heads[i];
        if (node != NULL) {
            do {
                if (node->follow != NULL) {
                    Vector_Add(node, node, transform);
                }
                node = node->next;
            } while (node != NULL);
        }
        i++;
    } while (i < 2);
}

/* Retargets the pool point at index, easing toward target over duration frames. */
void LinkedPoint_Retarget(f32 duration, s32 index, void *target) {
    LinkedPoint *point;

    if ((u32)index < LINKED_POINT_COUNT) {
        point = &linked_point_pool[index];
        point->duration = duration;
        point->position.w = 1.0f / duration;
        point->progress_scale = 1.0f / (duration * duration);
        Vector_Copy(&point->target, target);
        point->target.w = 1.0f;
    }
}

/* Takes the first node off the free list, clearing its links; returns NULL when empty. */
static LinkedPoint *LinkedPoint_TakeFree(void) {
    LinkedPoint *node;
    LinkedPoint *next;

    node = linked_point_free;
    if (node == NULL) {
        return NULL;
    }
    next = node->next;
    linked_point_free = next;
    if (next != NULL) {
        next->prev = NULL;
    }
    node->prev = NULL;
    node->next = NULL;
    return node;
}

/* Pushes a node onto the front of the free list. */
static void LinkedPoint_PushFree(LinkedPoint *node) {
    node->prev = NULL;
    node->next = linked_point_free;
    if (linked_point_free != NULL) {
        linked_point_free->prev = node;
    }
    linked_point_free = node;
}

/* Unlinks a point from its active list and returns the node that followed it. */
static LinkedPoint *LinkedPoint_UnlinkActive(LinkedPoint *node) {
    LinkedPoint *next;
    u8 list;

    next = node->next;
    list = node->list_index;
    if (node->prev != NULL) {
        node->prev->next = node->next;
    }
    if (node->next != NULL) {
        node->next->prev = node->prev;
    }
    if (linked_point_heads[list] == node) {
        linked_point_heads[list] = node->next;
    }
    node->prev = NULL;
    node->next = NULL;
    node->active = 0;
    return next;
}

/* Pushes a point onto the front of one of the two active lists (list is 0 or 1). */
static void LinkedPoint_LinkActive(LinkedPoint *point, u32 list) {
    LinkedPoint *head;

    if (list < 2) {
        head = linked_point_heads[list];
        point->active = TRUE;
        point->list_index = list;
        point->next = head;
        point->prev = NULL;
        if (linked_point_heads[list] != NULL) {
            linked_point_heads[list]->prev = point;
        }
        linked_point_heads[list] = point;
    }
}
