/* Script commands 200 to 299 (handler table D_00433650): arithmetic for scripts (sine, cosine, random
   numbers, angles and distances), whether the player is in control, the message box and notices, and the
   entities of the map (monsters and people), which a command names by spawn id (Entity_GetBySpawnId):
   their motions, walking and turning, their flags, positions and rotations. */
#include "kf_script.h"
#include "kf_entity.h"
#include "kf_entity_world.h"
#include "kf_game.h"
#include "kf_message.h"
#include "kf_player.h"

int Script_StoreFloatResult(ScriptArg *target, double value); // match: the definition takes a float, but retail converts the argument to double here (fptodp): there was no prototype in scope
int rand(void);
float atan2f(float, float);
double sqrt(double);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0ApplyMatrix(float *, float *, float *);
void Vector_Scale(void *, void *, float);
void Vector_Add(void *dest, void *a, void *b);
void Game_MarkMessageHeard(int flag);
int Motion_Start(void *, int, int, u32, int);
int Motion_Step(void *, int);
int Motion_BlendToPose(f32, void *); // match: retail leaves a value in $2
float EntityMath_HeadingTo(float *from, float *to);
float EntityMath_AngleDifference(float angle, float heading);
void EntityAi_SetState(char *, u8);
f32 EntityMath_Distance(f32 *a, f32 *b);
float EntityMath_DistanceXZ(void *a, void *b);
float EntityMath_WrapAngle(float angle);
float Angle_Wrap(float angle);

extern f32 D_0042C6F0[];
extern f32 D_00431AF0[16];
extern GameStatus D_0038CC50;
extern PlayerState D_00413F80;
extern Entity D_003BEDF0[][0x80];
extern s32 D_00446770;
extern s32 D_004467D8[2];

/* Command 200, and every unused number from 200 to 299: finishes at once. */
void ScriptCmd_Nop200(ScriptThreadCommand *command) {
    command->active = 0;
}

/* Command 201: finishes at once. */
void ScriptCmd_Nop201(ScriptThreadCommand *command) {
    command->active = 0;
}

/* Command 202: finishes at once. */
void ScriptCmd_Nop202(ScriptThreadCommand *command) {
    command->active = 0;
}

/* Command 203: finishes at once. */
void ScriptCmd_Nop203(ScriptThreadCommand *command) {
    command->active = 0;
}

/* Command 204: finishes at once. */
void ScriptCmd_Nop204(ScriptThreadCommand *command) {
    command->active = 0;
}

/* Command 205: finishes at once. */
void ScriptCmd_Nop205(ScriptThreadCommand *command) {
    command->active = 0;
}

/* Command 206: finishes at once. */
void ScriptCmd_Nop206(ScriptThreadCommand *command) {
    command->active = 0;
}

/* Command 207: finishes at once. */
void ScriptCmd_Nop207(ScriptThreadCommand *command) {
    command->active = 0;
}

/* Command 208: finishes at once. */
void ScriptCmd_Nop208(ScriptThreadCommand *command) {
    command->active = 0;
}

/* Command 209: finishes at once. */
void ScriptCmd_Nop209(ScriptThreadCommand *command) {
    command->active = 0;
}

/* Command 210: finishes at once. */
void ScriptCmd_Nop210(ScriptThreadCommand *command) {
    command->active = 0;
}

/* Command 211: finishes at once. */
void ScriptCmd_Nop211(ScriptThreadCommand *command) {
    command->active = 0;
}

/* Command 212: finishes at once. */
void ScriptCmd_Nop212(ScriptThreadCommand *command) {
    command->active = 0;
}

/* Command 213: finishes at once. */
void ScriptCmd_Nop213(ScriptThreadCommand *command) {
    command->active = 0;
}

/* Command 214: finishes at once. */
void ScriptCmd_Nop214(ScriptThreadCommand *command) {
    command->active = 0;
}

/* Command 215: finishes at once. */
void ScriptCmd_Nop215(ScriptThreadCommand *command) {
    command->active = 0;
}

/* Command 216: finishes at once. */
void ScriptCmd_Nop216(ScriptThreadCommand *command) {
    command->active = 0;
}

/* Command 217: finishes at once. */
void ScriptCmd_Nop217(ScriptThreadCommand *command) {
    command->active = 0;
}

/* Command 218: hands the script, through `result`, the sine of the angle `input` (radians), from the
   4096-step table D_0042C6F0, then finishes. */
void ScriptCmd_Sin(ScriptFloatQueryCommand *command) {
    ScriptArg *result = &command->result;
    int step = (int)(command->input.value * 4096.0f / 6.2831855f);
    int wrapped = step + 0x1000;
    int index = wrapped % 4096;

    Script_StoreFloatResult(result, D_0042C6F0[index]);
    command->active = 0;
}

/* Command 219: hands the script, through `result`, the cosine of the angle `input` (the sine table a
   quarter turn on), then finishes. */
void ScriptCmd_Cos(ScriptFloatQueryCommand *command) {
    ScriptArg *result = &command->result;
    int step = (int)(command->input.value * 4096.0f / 6.2831855f);
    int wrapped = step + 0x1000;
    int index = wrapped % 4096;

    Script_StoreFloatResult(result, D_0042C6F0[index + 0x400]);
    command->active = 0;
}

/* Command 226: hands the script, through `result`, a random number from 0 to below `input`, in steps of
   0.01, then finishes. */
void ScriptCmd_RandomFloat(ScriptFloatQueryCommand *command) {
    int hundredths;

    hundredths = command->input.value * 100.0f;
    Script_StoreFloatResult(&command->result, (f32)(rand() % hundredths) * 0.01f);
    command->active = 0;
}

/* Command 227: hands the script, through x, y and z, the player's position plus `distance` times the
   vector (PlayerState.unk1B0, unk1AC, unk1B4) turned by the player's heading, then finishes. */
void ScriptCmd_GetPointFromPlayer(ScriptPointAheadCommand *command) {
    f32 matrix[16];
    f32 vector[4];
    f32 point[4];

    vector[0] = D_00413F80.unk1B0;
    vector[1] = D_00413F80.unk1AC;
    vector[2] = D_00413F80.unk1B4;
    vector[3] = 0.0f;
    sceVu0RotMatrixY(matrix, D_00431AF0, D_00413F80.rotation.y);
    sceVu0ApplyMatrix(vector, matrix, vector);
    Vector_Scale(vector, vector, (f32)command->distance.value);
    Vector_Add(point, &D_00413F80.position, vector);
    Script_StoreFloatResult((ScriptArg *)&command->x, point[0]);
    Script_StoreFloatResult((ScriptArg *)&command->y, point[1]);
    Script_StoreFloatResult((ScriptArg *)&command->z, point[2]);
    command->active = 0;
}

/* Command 228: hands the script, through `angle`, atan2f(x, y), then finishes. */
void ScriptCmd_Atan2(ScriptAngleCommand *command) {
    Script_StoreFloatResult(&command->angle, atan2f(command->x.value, command->y.value));
    command->active = 0;
}

/* Command 229: hands the script, through `result`, a random number from 0 to below `input`, then
   finishes. */
void ScriptCmd_Random(ScriptQueryCommand *command) {
    Script_StoreResult(&command->result, rand() % command->input.value);
    command->active = 0;
}

/* Command 230: gives the player control (GameStatus.unk00 = 1: the player moves, the menus open), then
   finishes. */
void ScriptCmd_GivePlayerControl(ScriptThreadCommand *command) {
    D_0038CC50.unk00 = 1;
    command->active = 0;
}

/* Command 231: takes control from the player (GameStatus.unk00 = 0), then finishes. */
void ScriptCmd_TakePlayerControl(ScriptThreadCommand *command) {
    D_0038CC50.unk00 = 0;
    command->active = 0;
}

/* Command 232: marks the message `message_id` as heard, opens the message box with it and waits until the
   box has closed. */
void ScriptCmd_ShowMessageBox(ScriptMessageBoxCommand *command) {
    if (command->started == 0) {
        Game_MarkMessageHeard(command->message_id.value);
        Message_OpenBox(0, 0, command->font.value, 0, command->delay.value, command->message_id.value);
        command->started = 1;
    }
    if (Message_IsBoxOpen() == 0) {
        command->active = 0;
    }
}

/* Command 233: puts the message `message_id` up as a notice, then finishes. */
void ScriptCmd_ShowNotice(ScriptMessageCommand *command) {
    Message_ShowNotice(0, 0, command->font.value, command->kind.value, command->frames.value, command->message_id.value);
    command->active = 0;
}

/* Command 234: starts the motion `first` on the entity `target_id` (Motion_Start, blending over
   `second`), then finishes. */
void ScriptCmd_StartEntityMotion(ScriptCallCommand *command) {
    Entity *entity;

    entity = Entity_GetBySpawnId(command->target_id.value);
    if (entity != NULL && (entity->flags & 0xF) == 3 && entity->unk2C != 0) {
        Motion_Start(entity, command->first.value, 0, command->second.value, 1);
    }
    command->active = 0;
}

/* Command 235: plays the motion of the entity `target_id` a step on every frame (Motion_Step with
   `motion`) for `frames` frames. */
void ScriptCmd_PlayEntityMotion(ScriptMotionCommand *command) {
    Entity *entity;

    entity = Entity_GetBySpawnId(command->target_id.value);
    if (entity != NULL && (entity->flags & 0xF) == 3 && entity->unk2C != 0) {
        Motion_Step(entity, command->motion.value);
        if (--command->frames.value != 0) {
            return;
        }
    }
    command->active = 0;
}

/* Command 236: plays the motion of the entity `target_id` a step on every frame (Motion_Step with
   `first`) until it has ended. */
void ScriptCmd_WaitEntityMotion(ScriptCallCommand *command) {
    Entity *target;

    target = Entity_GetBySpawnId(command->target_id.value);
    if (target == NULL || (target->flags & 0xF) != 3 || target->unk2C == 0 ||
        Motion_Step(target, command->first.value) != 0) {
        command->active = 0;
    }
}

/* Command 237: blends the model of the entity `target_id` into its motion by `weight` every frame
   (Motion_BlendToPose) while the motion has blend frames. */
int ScriptCmd_BlendEntityMotion(ScriptMotionBlendCommand *command) {
    Entity *entity;
    EntityModel *entity_model;

    entity = Entity_GetBySpawnId(command->target_id.value);
    if (entity != NULL && (entity->flags & 0xF) == 3 && entity->unk2C != 0) {
        entity_model = entity->model;
        if (entity_model->unkFC->unk88 != 0) {
            return Motion_BlendToPose(command->weight.value, entity_model);
        }
    }
    command->active = 0;
}

/* Command 238: turns the entity `target_id` to face the player (state 4, EntityAi_SetState: on the spot for
   a kind with params flag 0x80, else through Entity.unkC8's turn bits), then waits `frames` frames and
   for state 5; done at once if it is in state 5 already, for the 0x80 kind only if it faces the player
   within params->unk438 degrees. */
void ScriptCmd_TurnEntityToPlayer(ScriptStateWaitCommand *command) {
    Entity *entity;
    f32 heading;
    f32 turn;

    entity = Entity_GetBySpawnId(command->target_id.value);
    if (entity != NULL && (entity->flags & 0xF) == 3 && entity->unk2C != 0) {
        if (entity->unkC6 != 0) {
            return;
        }
        if (entity->params->flags & 0x80) {
            switch (command->step) {
            case 0:
                heading = EntityMath_HeadingTo(entity->model->position, &D_00413F80.position.x);
                turn = EntityMath_AngleDifference(entity->model->unkD0[1], heading);
                if (entity->state == 5) {
                    if (ABS(turn) <= entity->params->unk438 * 0.017453292f) {
                        command->active = 0;
                        return;
                    }
                }
                EntityAi_SetState((char *)entity, 4);
                command->step++;
                return;
            case 1:
                if (--command->frames.value > 0) {
                    return;
                }
                if (entity->state != 5) {
                    return;
                }
                command->active = 0;
                return;
            default:
                return;
            }
        } else {
            switch (command->step) {
            case 0:
                if (entity->state == 5) {
                    command->active = 0;
                    return;
                }
                EntityAi_SetState((char *)entity, 4);
                command->step++;
                return;
            case 1:
                if (--command->frames.value > 0) {
                    return;
                }
                if (entity->state != 5) {
                    return;
                }
                command->active = 0;
                return;
            default:
                return;
            }
        }
    }
    command->active = 0;
}

/* Command 243: turns the entity `object_id` back to its own heading (state 6, EntityAi_SetState), then waits
   until it is in state 0. */
void ScriptCmd_TurnEntityBack(ScriptWaitCommand *command) {
    Entity *entity;

    entity = Entity_GetBySpawnId(command->object_id.value);
    if (entity != NULL && (entity->flags & 0xF) == 3 && entity->unk2C != 0) {
        if (entity->unkC6 != 0) {
            return;
        }
        if (command->started == 0) {
            EntityAi_SetState((char *)entity, 6);
            command->started = 1;
        }
        if (entity->state != 0) {
            return;
        }
    }
    command->active = 0;
}

/* Command 239: walks the entity `target_id` toward the point x, y, z until it is within `distance`: every
   frame it gives the entity the movement command to turn, climb or go down, and walk (or run). */
void ScriptCmd_WalkEntityTo(ScriptWalkToCommand *command) {
    f32 point[4];
    Entity *entity;
    f32 heading;
    f32 turn;
    f32 rise;

    entity = Entity_GetBySpawnId(command->target_id.value);
    if (entity != NULL) {
        point[0] = command->x.value;
        point[1] = command->y.value;
        point[2] = command->z.value;
        point[3] = 0.0f;
        if (EntityMath_Distance(point, entity->model->position) < command->distance.value) {
            command->active = 0;
            return;
        }
        if ((entity->flags & 0xF) == 3 && entity->unk2C != 0) {
            heading = EntityMath_HeadingTo(entity->model->position, point);
            turn = EntityMath_AngleDifference(entity->model->unkD0[1], heading);
            if (ABS(turn) > entity->params->turn_limit) {
                if (turn >= 0.0f) {
                    entity->unkC8 |= 0x200;
                } else {
                    entity->unkC8 |= 0x100;
                }
            }
            rise = point[1] - entity->model->position[1];
            if (ABS(rise) > entity->params->unk54) {
                if (rise >= 0.0f) {
                    entity->unkC8 |= 0x20;
                } else {
                    entity->unkC8 |= 0x10;
                }
            }
            entity->unkC8 |= 1;
            if (command->run.value != 0) {
                entity->unkC8 |= 0x40;
            }
            ENTITY_FLAG_BITS(entity)->bits11 = command->mode.value;
            return;
        }
    }
    command->active = 0;
}

/* Command 240: command 239 on the ground: walks the entity `target_id` toward the point x, z until it is
   within `distance` of it, heights left out. */
void ScriptCmd_WalkEntityToFlat(ScriptWalkToFlatCommand *command) {
    f32 point[4];
    Entity *entity;
    f32 heading;
    f32 turn;

    entity = Entity_GetBySpawnId(command->target_id.value);
    if (entity != NULL) {
        point[0] = command->x.value;
        point[2] = command->z.value;
        point[1] = 0.0f;
        point[3] = 0.0f;
        if (EntityMath_DistanceXZ(point, entity->model->position) < command->distance.value) {
            command->active = 0;
            return;
        }
        if ((entity->flags & 0xF) == 3 && entity->unk2C != 0) {
            heading = EntityMath_HeadingTo(entity->model->position, point);
            turn = EntityMath_AngleDifference(entity->model->unkD0[1], heading);
            if (ABS(turn) > entity->params->turn_limit) {
                if (turn >= 0.0f) {
                    entity->unkC8 |= 0x200;
                } else {
                    entity->unkC8 |= 0x100;
                }
            }
            entity->unkC8 |= 1;
            if (command->run.value != 0) {
                entity->unkC8 |= 0x40;
            }
            ENTITY_FLAG_BITS(entity)->bits11 = command->mode.value;
            return;
        }
    }
    command->active = 0;
}

/* Command 241: turns the entity `target_id` by x, y, z every frame for `frames` frames, playing its motion
   on (`motion`) when `animate` is set. */
void ScriptCmd_TurnEntityBy(ScriptMoveCommand *command) {
    Entity *target;

    target = Entity_GetBySpawnId(command->target_id.value);
    if (target != NULL && (target->flags & 0xF) == 3 && target->unk2C != 0) {
        ENTITY_MODEL(target)->rotation.x = EntityMath_WrapAngle(ENTITY_MODEL(target)->rotation.x + command->x.value);
        ENTITY_MODEL(target)->rotation.y = EntityMath_WrapAngle(ENTITY_MODEL(target)->rotation.y + command->y.value);
        ENTITY_MODEL(target)->rotation.z = EntityMath_WrapAngle(ENTITY_MODEL(target)->rotation.z + command->z.value);
        if (command->animate.value != 0) {
            Motion_Step(target, command->motion.value);
        }
        if (--command->frames.value != 0) {
            return;
        }
    }
    command->active = 0;
}

/* Command 242: turns the entity `target_id` to face the point x, z over `frames` frames, playing its
   motion on (`motion`) when `animate` is set. */
void ScriptCmd_TurnEntityToward(ScriptMoveCommand *command) {
    Entity *target;
    f32 heading;

    target = Entity_GetBySpawnId(command->target_id.value);
    if (target == NULL || (target->flags & 0xF) != 3 || target->unk2C == 0) {
        command->active = 0;
        return;
    }
    heading = Angle_Wrap(atan2f(command->x.value - ENTITY_MODEL(target)->position.x,
                                   command->z.value - ENTITY_MODEL(target)->position.z) + 3.1415927f);
    if (heading == ENTITY_MODEL(target)->rotation.y) {
        command->active = 0;
        return;
    }
    ENTITY_MODEL(target)->rotation.y += Angle_Wrap(heading - ENTITY_MODEL(target)->rotation.y) / (f32)command->frames.value;
    if (command->animate.value != 0) {
        Motion_Step(target, command->motion.value);
    }
    if (--command->frames.value == 0) {
        command->active = 0;
    }
}

/* Command 244: copies bit 0 of `value` into flag bit 5 of the current map's entities: with `kind` 1 only
   of those whose flag bit 16 is clear, with 2 only of those where it is set, else of all. Then
   finishes. */
void ScriptCmd_SetEntitiesFlag5(ScriptFlagCommand *command) {
    Entity *entity = D_003BEDF0[D_00446770];
    s32 i;

    i = D_004467D8[D_00446770];
    while (--i != -1) {
        if (command->kind.value == 1) {
            if (!ENTITY_FLAG_BITS(entity)->bit16) {
                ENTITY_FLAG_BITS(entity)->bit5 = command->value.value;
            }
        } else if (command->kind.value == 2) {
            if (ENTITY_FLAG_BITS(entity)->bit16) {
                ENTITY_FLAG_BITS(entity)->bit5 = command->value.value;
            }
        } else {
            ENTITY_FLAG_BITS(entity)->bit5 = command->value.value;
        }
        entity++;
    }
    command->active = 0;
}

/* Command 245: hands the script, through `result`, whether the entity `target_id` faces within `range` / 2
   degrees of its Entity.unk68 heading (FALSE when there is no such entity), then finishes. */
void ScriptCmd_IsEntityFacing(ScriptFacingCommand *command) {
    Entity *entity;
    f32 angle;

    entity = Entity_GetBySpawnId(command->target_id.value);
    if (entity != NULL) {
        angle = Angle_Wrap(entity->unk68 - entity->model->unkD0[1]);
        angle = ABS(angle);
        angle *= 57.29578f;
        if (angle <= command->range.value / 2) {
            Script_StoreResult(&command->result, TRUE);
            command->active = 0;
            return;
        }
    }
    Script_StoreResult(&command->result, FALSE);
    command->active = 0;
}

/* Command 246: sets how fast the entity `target_id` animates (Entity.unkD6, 0x80 the normal speed), then
   finishes. */
void ScriptCmd_SetEntityAnimSpeed(ScriptCallCommand *command) {
    Entity *entity;

    entity = Entity_GetBySpawnId(command->target_id.value);
    if (entity != NULL) {
        entity->unkD6 = command->first.value;
        entity->model->unk15C = entity->unkD6;
    }
    command->active = 0;
}

/* Command 247: hands the script the position of the entity `target_id` through x, y and z (0 when there
   is no such entity), then finishes. */
void ScriptCmd_GetEntityPosition(ScriptEntityVectorCommand *command) {
    Entity *entity;

    entity = Entity_GetBySpawnId(command->target_id.value);
    if (entity != NULL) {
        Script_StoreFloatResult((ScriptArg *)&command->x, entity->model->position[0]);
        Script_StoreFloatResult((ScriptArg *)&command->y, entity->model->position[1]);
        Script_StoreFloatResult((ScriptArg *)&command->z, entity->model->position[2]);
    } else {
        Script_StoreFloatResult((ScriptArg *)&command->x, 0);
        Script_StoreFloatResult((ScriptArg *)&command->y, 0);
        Script_StoreFloatResult((ScriptArg *)&command->z, 0);
    }
    command->active = 0;
}

/* Command 248: hands the script the rotation of the entity `target_id` through x, y and z (0 when there
   is no such entity), then finishes. */
void ScriptCmd_GetEntityRotation(ScriptEntityVectorCommand *command) {
    Entity *entity;

    entity = Entity_GetBySpawnId(command->target_id.value);
    if (entity != NULL) {
        Script_StoreFloatResult((ScriptArg *)&command->x, entity->model->unkD0[0]);
        Script_StoreFloatResult((ScriptArg *)&command->y, entity->model->unkD0[1]);
        Script_StoreFloatResult((ScriptArg *)&command->z, entity->model->unkD0[2]);
    } else {
        Script_StoreFloatResult((ScriptArg *)&command->x, 0);
        Script_StoreFloatResult((ScriptArg *)&command->y, 0);
        Script_StoreFloatResult((ScriptArg *)&command->z, 0);
    }
    command->active = 0;
}

/* Command 249: hands the script the position the entity `target_id` spawned at through x, y and z (0 when
   there is no such entity), then finishes. */
void ScriptCmd_GetEntitySpawnPosition(ScriptEntityVectorCommand *command) {
    Entity *entity;

    entity = Entity_GetBySpawnId(command->target_id.value);
    if (entity != NULL) {
        Script_StoreFloatResult((ScriptArg *)&command->x, entity->spawn_info->spawn_x);
        Script_StoreFloatResult((ScriptArg *)&command->y, entity->spawn_info->spawn_y);
        Script_StoreFloatResult((ScriptArg *)&command->z, entity->spawn_info->spawn_z);
    } else {
        Script_StoreFloatResult((ScriptArg *)&command->x, 0);
        Script_StoreFloatResult((ScriptArg *)&command->y, 0);
        Script_StoreFloatResult((ScriptArg *)&command->z, 0);
    }
    command->active = 0;
}

/* Command 250: hands the script the rotation the entity `target_id` spawned with through x, y and z (0
   when there is no such entity), then finishes. */
void ScriptCmd_GetEntitySpawnRotation(ScriptEntityVectorCommand *command) {
    Entity *entity;

    entity = Entity_GetBySpawnId(command->target_id.value);
    if (entity != NULL) {
        Script_StoreFloatResult((ScriptArg *)&command->x, entity->spawn_info->unk10);
        Script_StoreFloatResult((ScriptArg *)&command->y, entity->spawn_info->unk14);
        Script_StoreFloatResult((ScriptArg *)&command->z, entity->spawn_info->unk18);
    } else {
        Script_StoreFloatResult((ScriptArg *)&command->x, 0);
        Script_StoreFloatResult((ScriptArg *)&command->y, 0);
        Script_StoreFloatResult((ScriptArg *)&command->z, 0);
    }
    command->active = 0;
}

/* Command 251: hands the script, through `result`, the distance between the points a and b in the plane,
   then finishes. */
void ScriptCmd_Distance2D(ScriptDistance2DCommand *command) {
    f32 delta_x;
    f32 delta_y;
    f32 distance_sq;
    f32 distance;

    delta_x = command->a_x.value - command->b_x.value;
    delta_y = command->a_y.value - command->b_y.value;
    distance_sq = delta_x * delta_x + delta_y * delta_y;
    distance = sqrt(distance_sq);
    Script_StoreFloatResult(&command->result, distance);
    command->active = 0;
}

/* Command 252: hands the script, through `result`, the distance between the points a and b, then
   finishes. */
void ScriptCmd_Distance(ScriptDistanceCommand *command) {
    f32 delta_x;
    f32 delta_y;
    f32 delta_z;
    f32 distance_sq;
    f32 distance;

    delta_x = command->a_x.value - command->b_x.value;
    delta_y = command->a_y.value - command->b_y.value;
    delta_z = command->a_z.value - command->b_z.value;
    distance_sq = delta_x * delta_x + delta_y * delta_y + delta_z * delta_z;
    distance = sqrt(distance_sq);
    Script_StoreFloatResult(&command->result, distance);
    command->active = 0;
}

/* Command 253: sets sixteen flags of the entity `target_id`: for each bit set in `mask`, the same bit of
   `value` goes to one of the flags (the bits command 254 reads back). Then finishes. */
void ScriptCmd_SetEntityFlags(ScriptSetFlagsCommand *command) {
    Entity *entity;

    entity = Entity_GetBySpawnId(command->target_id.value);
    if (entity == NULL) {
        command->active = 0;
        return;
    }
    if (command->mask.value & 1) {
        ENTITY_FLAG_BITS(entity)->bit1 = command->value.value & 1;
    }
    if (command->mask.value & 2) {
        ENTITY_FLAG_BITS(entity)->bits2 = (command->value.value & 2) ? 3 : 0;
    }
    if (command->mask.value & 0x4) {
        ENTITY_FLAG_BITS(entity)->bit5 = command->value.value >> 2;
    }
    if (command->mask.value & 0x8) {
        ENTITY_FLAG_BITS(entity)->bit6 = command->value.value >> 3;
    }
    if (command->mask.value & 0x10) {
        ENTITY_FLAG_BITS(entity)->bit7 = command->value.value >> 4;
    }
    if (command->mask.value & 0x20) {
        ENTITY_FLAG_BITS(entity)->bit18 = command->value.value >> 5;
    }
    if (command->mask.value & 0x40) {
        ENTITY_FLAG_BITS(entity)->bit22 = command->value.value >> 6;
    }
    if (command->mask.value & 0x80) {
        ENTITY_FLAG_BITS(entity)->bit23 = command->value.value >> 7;
    }
    if (command->mask.value & 0x100) {
        ENTITY_FLAG_BITS(entity)->bit24 = command->value.value >> 8;
    }
    if (command->mask.value & 0x200) {
        ENTITY_FLAG_BITS(entity)->bit25 = command->value.value >> 9;
    }
    if (command->mask.value & 0x400) {
        ENTITY_FLAG_BITS(entity)->bit26 = command->value.value >> 10;
    }
    if (command->mask.value & 0x800) {
        ENTITY_FLAG_BITS(entity)->bit27 = command->value.value >> 11;
    }
    if (command->mask.value & 0x1000) {
        ENTITY_FLAG_BITS(entity)->bit28 = command->value.value >> 12;
    }
    if (command->mask.value & 0x2000) {
        ENTITY_FLAG_BITS(entity)->bit14 = command->value.value >> 13;
    }
    if (command->mask.value & 0x4000) {
        ENTITY_FLAG_BITS(entity)->bit9 = command->value.value >> 14;
    }
    if (command->mask.value & 0x8000) {
        ENTITY_FLAG_BITS(entity)->bit29 = command->value.value >> 15;
    }
    command->active = 0;
}

/* Command 254: hands the script, through `result`, sixteen flags of the entity `input` packed into one
   word (the bits command 253 sets; bit 1 when flag bits 2-3 are 2 or 3), then finishes. */
void ScriptCmd_GetEntityFlags(ScriptQueryCommand *command) {
    Entity *entity;
    int result;

    entity = Entity_GetBySpawnId(command->input.value);
    if (entity != NULL) {
        result = ENTITY_FLAG_BITS(entity)->bit1;
        if (ENTITY_FLAG_BITS(entity)->bits2 >= 2) {
            result |= 2;
        }
        if (ENTITY_FLAG_BITS(entity)->bit5) {
            result |= 0x4;
        }
        if (ENTITY_FLAG_BITS(entity)->bit6) {
            result |= 0x8;
        }
        if (ENTITY_FLAG_BITS(entity)->bit7) {
            result |= 0x10;
        }
        if (ENTITY_FLAG_BITS(entity)->bit18) {
            result |= 0x20;
        }
        if (ENTITY_FLAG_BITS(entity)->bit22) {
            result |= 0x40;
        }
        if (ENTITY_FLAG_BITS(entity)->bit23) {
            result |= 0x80;
        }
        if (ENTITY_FLAG_BITS(entity)->bit24) {
            result |= 0x100;
        }
        if (ENTITY_FLAG_BITS(entity)->bit25) {
            result |= 0x200;
        }
        if (ENTITY_FLAG_BITS(entity)->bit26) {
            result |= 0x400;
        }
        if (ENTITY_FLAG_BITS(entity)->bit27) {
            result |= 0x800;
        }
        if (ENTITY_FLAG_BITS(entity)->bit28) {
            result |= 0x1000;
        }
        if (ENTITY_FLAG_BITS(entity)->bit14) {
            result |= 0x2000;
        }
        if (ENTITY_FLAG_BITS(entity)->bit9) {
            result |= 0x4000;
        }
        if (ENTITY_FLAG_BITS(entity)->bit29) {
            result |= 0x8000;
        }
        Script_StoreResult(&command->result, result);
    }
    command->active = 0;
}

/* Command 255: puts the entity `target_id` at the position x, y, z, then finishes. */
void ScriptCmd_SetEntityPosition(ScriptEntityVectorCommand *command) {
    Entity *entity;

    entity = Entity_GetBySpawnId(command->target_id.value);
    if (entity != NULL) {
        ENTITY_MODEL(entity)->position.x = command->x.value;
        ENTITY_MODEL(entity)->position.y = command->y.value;
        ENTITY_MODEL(entity)->position.z = command->z.value;
    }
    command->active = 0;
}

/* Command 256: sets the rotation of the entity `target_id` to x, y, z (each wrapped by Angle_Wrap),
   then finishes. */
void ScriptCmd_SetEntityRotation(ScriptEntityVectorCommand *command) {
    Entity *entity;

    entity = Entity_GetBySpawnId(command->target_id.value);
    if (entity != NULL) {
        entity->model->unkD0[0] = command->x.value;
        entity->model->unkD0[1] = command->y.value;
        entity->model->unkD0[2] = command->z.value;
        entity->model->unkD0[0] = Angle_Wrap(entity->model->unkD0[0]);
        entity->model->unkD0[1] = Angle_Wrap(entity->model->unkD0[1]);
        entity->model->unkD0[2] = Angle_Wrap(entity->model->unkD0[2]);
    }
    command->active = 0;
}

/* Command 257: sets flag bit 18 of the entity `input` when its flag bits 0-1 are 3, Entity.unk82 is 0 and
   flag bits 2, 3, 36 and 37 are clear; hands the script, through `result`, whether it did. Then
   finishes. */
void ScriptCmd_TrySetEntityFlag18(ScriptQueryCommand *command) {
    Entity *entity;
    s64 flags;

    entity = Entity_GetBySpawnId(command->input.value);
    if (entity != NULL) {
        flags = entity->flags;
        if (((flags & 3) == 3) && (entity->unk82 == 0) && ((flags & ((0x30L << 32) | 0xC)) == 0)) {
            entity->flags = flags | 0x40000;
            Script_StoreResult(&command->result, TRUE);
            command->active = 0;
            return;
        }
    }
    Script_StoreResult(&command->result, FALSE);
    command->active = 0;
}

/* Command 258: clears flag bit 18 of the entity `object_id` on the same terms as command 257, then
   finishes. */
void ScriptCmd_ClearEntityFlag18(ScriptObjectCommand *command) {
    Entity *entity;
    s64 flags;

    entity = Entity_GetBySpawnId(command->object_id.value);
    if (entity != NULL) {
        flags = entity->flags;
        if (((flags & 3) == 3) && (entity->unk82 == 0) && ((flags & ((0x30L << 32) | 0xC)) == 0)) {
            entity->flags = flags & ~0x40000;
        }
    }
    command->active = 0;
}
