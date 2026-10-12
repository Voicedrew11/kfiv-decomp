#include "kf_game.h"

void sceVu0CopyVector(f32 *, f32 *);

/* Copies a part file with every strip point expanded to its position and normal (0x40 bytes a point);
   the strips go right behind the copy's parts. */
void func_00203C88(PoolPartFile *source, PoolPartFile *dest) {
    PoolPart *source_part;
    PoolPart *dest_part;
    PoolPartFlatStrip *dest_strip;
    PoolPartMaterial *source_strip;
    PoolPartFlatPoint *dest_point;
    PoolPartPoint *source_point;
    f32 (*normals)[4];
    f32 (*points)[4];
    s32 part_count;
    s32 strip_count;
    s32 point_count;

    *(PoolPartFileHeader *)dest = *(PoolPartFileHeader *)source;
    part_count = source->part_count;
    dest_part = dest->parts;
    source_part = source->parts;
    dest_strip = (PoolPartFlatStrip *)(dest_part + part_count);
    while (--part_count != -1) {
        *dest_part = *source_part;
        source_strip = source_part->material;
        strip_count = dest_part->strip_count;
        points = source_part->points;
        normals = source_part->normals;
        dest_part->material = (PoolPartMaterial *)dest_strip;
        while (--strip_count != -1) {
            // match: the strip's 0x20 bytes go over a word at a time through a union, between the other statements
            ((PoolPartStripWords *)dest_strip)->words[0] = ((PoolPartStripWords *)source_strip)->words[0];
            dest_point = dest_strip->points;
            source_point = source_strip->points;
            ((PoolPartStripWords *)dest_strip)->words[1] = ((PoolPartStripWords *)source_strip)->words[1];
            point_count = dest_strip->point_count;
            ((PoolPartStripWords *)dest_strip)->words[2] = ((PoolPartStripWords *)source_strip)->words[2];
            ((PoolPartStripWords *)dest_strip)->words[3] = ((PoolPartStripWords *)source_strip)->words[3];
            while (--point_count != -1) {
                sceVu0CopyVector(dest_point->position, points[source_point->point]);
                sceVu0CopyVector(dest_point->normal, normals[source_point->normal]);
                dest_point->tex_s = source_point->tex_s;
                dest_point->tex_t = source_point->tex_t;
                dest_point->tex_q = 1.0f;
                dest_point->unk2C = 0.0f;
                dest_point->color[0] = source_point->color[0];
                dest_point->color[1] = source_point->color[1];
                dest_point->color[2] = source_point->color[2];
                dest_point->color[3] = source_point->color[3];
                source_point++;
                dest_point++;
            }
            source_strip = (PoolPartMaterial *)source_point;
            dest_strip = (PoolPartFlatStrip *)dest_point;
        }
        source_part++;
        dest_part++;
    }
}
