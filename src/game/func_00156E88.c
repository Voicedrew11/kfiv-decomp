#include "kf_entity.h"
#include "kf_map_object.h"

extern int D_00446770;

float func_00194FB8(float);
float func_00194F80(void *, void *);
int func_00192C98(void *, int, int, u32, int);
int func_001958B8(Entity *, void (*)(Entity *), int);

/* walks through one of three doorways (map objects 0xA0-0xA2): unk7A says which. It is put just
   inside the doorway facing through it, opens the door on an animation frame and walks to a point
   on the far side. */
void func_00156E88(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    void *map_object;
    f32 coord; /* also the reach tested in step 4 */
    f32 centre;
    f32 limit;
    f32 angle;

    entity->unkC6 |= 0x1000;
    switch (entity->action_step) {
    case 0:
        ENTITY_FLAG_BITS(entity)->bit23 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit28 = TRUE;
        switch (unkE0->unk7A) {
        case 1:
            centre = 4200.0f;
            coord = (entity->model->position[0] > centre ? entity->radius + 50.0f : -(entity->radius + 50.0f)) + centre;
            entity->model->position[0] = coord;
            entity->model->position[1] = 640.0f;
            entity->model->position[2] = -2400.0f;
            entity->model->position[3] = 0.0f;
            centre = 4200.0f;
            angle = -1.5707964f;
            if (entity->model->position[0] > centre) {
                angle = 1.5707964f;
            }
            entity->model->unkD0[1] = angle;
            coord = (entity->model->position[0] > centre ? -(entity->radius + 100.0f) : entity->radius + 100.0f) + centre;
            unkE0->unk30[0] = coord;
            unkE0->unk30[1] = 640.0f;
            unkE0->unk30[2] = -2400.0f;
            unkE0->unk30[3] = 0.0f;
            break;
        case 2:
            centre = -1650.0f;
            coord = (entity->model->position[2] > centre ? entity->radius + 50.0f : -(entity->radius + 50.0f)) + centre;
            limit = -1650.0f;
            entity->model->position[0] = 5200.0f;
            entity->model->position[1] = 640.0f;
            entity->model->position[2] = coord;
            entity->model->position[3] = 0.0f;
            entity->model->unkD0[1] = func_00194FB8(entity->model->position[2] > limit ? 0.0f : 3.1415927f);
            coord = (entity->model->position[2] > limit ? -(entity->radius + 100.0f) : entity->radius + 100.0f) + limit;
            unkE0->unk30[0] = 5200.0f;
            unkE0->unk30[1] = 640.0f;
            unkE0->unk30[2] = coord;
            unkE0->unk30[3] = 0.0f;
            break;
        case 3:
            centre = -1650.0f;
            coord = (entity->model->position[2] > centre ? entity->radius + 50.0f : -(entity->radius + 50.0f)) + centre;
            limit = -1650.0f;
            entity->model->position[0] = 3400.0f;
            entity->model->position[1] = 640.0f;
            entity->model->position[2] = coord;
            entity->model->position[3] = 0.0f;
            entity->model->unkD0[1] = func_00194FB8(entity->model->position[2] > limit ? 0.0f : 3.1415927f);
            coord = (entity->model->position[2] > limit ? -(entity->radius + 100.0f) : entity->radius + 100.0f) + limit;
            unkE0->unk30[0] = 3400.0f;
            unkE0->unk30[1] = 640.0f;
            unkE0->unk30[2] = coord;
            unkE0->unk30[3] = 0.0f;
            break;
        }
        func_00192C98(entity, 0x11, 0, 0, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (entity->model->unkFC->unk82 == entity->params->unk43C) {
            switch (unkE0->unk7A) {
            case 1:
                if (entity->group_id == D_00446770) {
                    map_object = MapObject_GetById(entity->group_id, 0xA2);
                    if (map_object != NULL) {
                        MapObject_Break(map_object);
                    }
                }
                break;
            case 2:
                if (entity->group_id == D_00446770) {
                    map_object = MapObject_GetById(entity->group_id, 0xA0);
                    if (map_object != NULL) {
                        MapObject_Break(map_object);
                    }
                }
                break;
            case 3:
                if (entity->group_id == D_00446770) {
                    map_object = MapObject_GetById(entity->group_id, 0xA1);
                    if (map_object != NULL) {
                        MapObject_Break(map_object);
                    }
                }
                break;
            }
        }
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            func_00192C98(entity, 3, 0, entity->params->unkC4, 1);
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
            entity->action_step++;
        }
        break;
    case 3:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
            ENTITY_FLAG_BITS(entity)->bits11 = 1;
        }
        break;
    case 4:
        coord = 100.0f;
        if (func_00194F80(entity->model->position, unkE0->unk30) < coord) {
            ENTITY_FLAG_BITS(entity)->bit23 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            unkE0->unk7A = 0;
            if (ENTITY_FLAG_BITS(entity)->bit5) {
                unkE0->unk00(entity, 1);
            } else {
                func_001958B8((char *)entity, unkE0->unk18, 1);
            }
        }
        break;
    }
}
