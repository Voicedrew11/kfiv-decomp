 
 



 



typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;
typedef long s64;                
typedef unsigned long u64;
typedef float f32;
typedef double f64;












 





 
typedef struct {
    f32 x;
    f32 y;
    f32 z;
    f32 w;
} Vector4;

 
typedef struct {
    f32 x;
    f32 y;
    f32 z;
    f32 w;
} __attribute__((aligned(16))) VuVector4;

 
typedef struct {
    f32 x;
    f32 y;
    f32 z;
    f32 w;
} __attribute__((aligned(8))) Vector4Arg;

 
typedef struct {
    u64 unk00;
    u64 unk08;
} Qword;

 
typedef struct __attribute__((packed)) {
    u64 unk00;
    u64 unk08;
} UnalignedQword;

 
typedef s32 VuIntVector[4] __attribute__((aligned(16)));

 
typedef u32 Uint128 __attribute__((mode(TI)));

 
typedef struct {
    void *unk00;                     
    s32 unk04;                       
    s32 unk08;                       
    s32 unk0C;                       
} StripVertex;

 
typedef struct {
    u16 vertex_count;                
    char pad02[0x1E];
    StripVertex vertices[1];       
} Strip;

 
typedef struct {
    s32 strip_count;                 
    Strip *strips;                   
    s32 unk08;                       
    u32 unk0C;                       
} ModelStripList;

 
typedef struct ModelPart {
    char unk00[0x40];                
    f32 unk40[4];                    
    f32 unk50;                       
    f32 unk54;                       
    f32 unk58;                       
    f32 unk5C;                       
    f32 unk60[3];                    
    u8 unk6C;                        
    char pad6D[0x03];
    struct ModelPart *child;         
    struct ModelPart *sibling;       
    u64 data_addr : 32;              
    u64 unk78_32 : 7;                
    u64 level : 7;                   
    u64 pad78_46 : 7;
    u64 data_count : 11;
} ModelPart;

 
typedef struct {
    s16 x;                           
    s16 y;                           
    s16 z;                           
    char pad06[0x08];
    u16 unk0E;                       
    u16 tex_u;                           
    u16 tex_v;                           
    u16 unk14;                       
    char pad16[0x02];
    s16 colour[4];                   
} ModelPartVertex;

 
typedef struct {
    s16 x;                           
    s16 y;                           
    s16 z;                           
    s16 unk06;                       
    s16 unk08[3];                    
    u16 unk0E;                       
    s16 tex_u;                           
    s16 tex_v;                           
    s16 unk14;                       
    s16 unk16;                       
    s16 colour[4];                   
} ModelPartVertexWords;

 
typedef struct {
    s32 unk00;                       
    char pad04[0x0C];
    ModelPartVertex vertices[1];     
} ModelPartDataTail;

 
typedef struct {
    u64 pad0 : 37;
    u64 unk37 : 14;                  
    u64 pad51 : 10;
    u64 unk61 : 3;                   
} ModelPartDataBits30;

 
typedef struct {
    u16 length;                      
    char pad02[0x2E];
    ModelPartDataBits30 unk30;       
    char pad38[0x08];
    u64 unk40;                       
    char pad48[0x08];
    u64 unk50;                       
    char pad58[0x18];
    s32 unk70;                       
    s32 unk74;                       
    char pad78[0x18];
    ModelPartVertex vertices[1];     
} ModelPartData;

 
typedef struct {
    char pad00[0x40];
    int id_word;                     
    char pad44[0x1C];
    int payload_size;                
    char pad64[0x0C];
} ModelPartRecord;

 
typedef struct {
    char pad00[0x78];
    u64 unk78;                       
} ModelPartWords;



 
typedef union {
    u64 word;
    u32 halves[2];
} ModelPartWordUnion;



 


 
typedef struct {
    f32 position[4];                 
    f32 offset[4];                   
    f32 tex_s;                           
    f32 tex_t;                           
    char pad28[0x04];
    s32 on_parent;                   
    char pad30[0x10];
} ModelWideVertex;

 
typedef struct {
    u16 length;                      
    char pad02[0x8E];
    s32 unk90;                       
    char pad94[0x1C];
    ModelWideVertex vertices[1];     
} ModelWideData;

 
typedef struct {
    u64 pad0 : 53;
    u64 blended : 1;                 
    u64 pad54 : 10;
} ModelPartDataBits70;

 
typedef struct {
    f32 position[3];                 
    f32 rotation[3];                 
    f32 scale[3];                    
    s32 data_offset;                 
    u16 data_count;                  
    char pad2A[0x06];
} PosedModelFilePart;

 
typedef struct {
    s32 size;                        
    u16 part_count;                  
    char pad06[0x06];
    s32 bone_anchors_offset;         
    PosedModelFilePart parts[1];     
} PosedModelFile;

 
typedef struct {
    char pad00[0x68];
    u64 pad68_0 : 40;
    u64 bit40 : 1;                   
    u64 bit41 : 1;                   
    u64 pad68_42 : 22;
} ModelPartFlagBits;



 
typedef struct {
    char pad00[0x68];
    u64 unk68;                       
} ModelPartFlagWord;





 
typedef struct {
    u8 count;                        
    char pad01;
    u8 unk02;                        
    u8 child;                        
    u8 sibling;                      
    char pad05[0x03];
    u32 keys_offset;                 
    u16 *thresholds;                 
} ModelPartLevels;

 
typedef struct {
    u16 frame;                       
    u16 length;                      
    u16 position;                    
    u16 position_in;                 
    u16 position_out;                
    u16 rotation;                    
    u16 rotation_in;                 
    u16 rotation_out;                
    u16 scale;                       
} MotionKey;

 
typedef struct {
    f32 x;
    f32 y;
    f32 z;
} MotionVector;

 
typedef struct {
    u32 size;                        
    u32 vectors_offset;              
    MotionVector *vectors;           
    u16 frame_count;                 
    u8 part_count;                   
    char pad0F;
    ModelPartLevels parts[1];        
} MotionData;

 
typedef struct {
    char pad00[0x10];
} MotionFileEntry;

 
typedef struct {
    char pad00[0x04];
    u32 motion_count;                
    char pad08[0x08];
    MotionFileEntry entries[1];      
} MotionFile;

 
typedef struct {
    f32 offset[3];                   
    u8 bone;                         
    char pad0D[0x03];
} BoneAnchor;

 
typedef struct {
    u16 unk00;                       
    char pad02[0x02];
} ModelTableHeader;

 
typedef struct {
    s32 data_offset;                 
    u16 data_count;                  
    u8 unk06;                        
    s8 unk07;                        
    char pad08[0x08];
} ModelFilePart;

 
typedef struct ModelFile {
    s32 size;                        
    u16 part_count;                  
    char pad06[0x0A];
    BoneAnchor bone_anchors[32];     
    ModelFilePart parts[1];          
} ModelFile;

 
typedef union {
    ModelFile plain;
    PosedModelFile posed;
} ModelFileUnion;





 
typedef struct {
    char pad00[0x130];
    ModelPart *parts[3];             
} ModelInstanceSlots;



 
typedef struct ModelWaveUv {
    u16 tex_u;                           
    u16 tex_v;                           
    s32 wave;                        
} ModelWaveUv;

typedef struct ModelInstance {
    char pad00[0x40];
    char unk40[0x40];                
    char unk80[0x40];                
    Vector4 position;                
    Vector4 rotation;                
    f32 unkE0;                       
    f32 unkE4;                       
    f32 unkE8;                       
    f32 unkEC;                       
    u16 part_count;                  
    u16 unkF2;                       
    char padF4[0x04];
    ModelPart *parts;                
    ModelPartLevels **level_tables;  
    u16 unk100;                      
    u16 unk102;                      
    int unk104;                      
    char pad108[0x08];
    ModelStripList *strips;          
    MotionFile *motion_file;         
    char pad118[0x04];
    ModelFile *file;                 
    BoneAnchor *bone_anchors;        
    Vector4 *bone_positions;         
    struct ModelInstance *draw_next[2];  
    ModelPart *unk130;               
    char pad134[0x08];
    f32 radius;                      
    u8 hit_handles[4];               
    u8 hit_count;                    
    u8 unk145;                       
    s16 unk146;                      
    s32 unk148;                      
    void *unk14C;                    
    f32 unk150;                      
    f32 unk154;                      
    f32 unk158;                      
    f32 alpha;                       
    s32 unk160[4];                   
    char pad170[0x08];
    void *unk178;                    
    int found_record_id;             
    int found_record[4];             
    u16 unk190;                      
    s16 unk192;                      
    int unk194;                      
    int unk198;                      
    int unk19C;                      
    int unk1A0;                      
    struct ModelWaveUv *wave_uvs;    
    void (*unk1A8)(struct ModelInstance *model);     
    void (*draw)(struct ModelInstance *model);   
    void (*unk1B0)();                
    void (*unk1B4)(struct ModelInstance *model);     
    int (*get_point)(void *model, int index);    
    void (*unk1BC)(struct ModelInstance *model);     
} ModelInstance;

 
typedef struct {
    f32 unk00[4];                    
    f32 unk10[4];                    
    f32 unk20[4];                    
    f32 unk30[4];                    
} BlendVertex;

 
typedef struct {
    f32 unk00[4];                    
    f32 unk10[4];                    
    char pad20[0x04];
    f32 unk24;                       
    char pad28[0x18];
} __attribute__((aligned(16))) ModelLight;

 
typedef struct {
    s32 x;                           
    s32 y;                           
    u32 z;                           
    s32 unk0C;                       
} ModelScreenPoint;

 
typedef struct {
    f32 rows[4][4];
} __attribute__((aligned(16))) ModelMatrix;

 
typedef struct {
    Uint128 rows[4];
} ModelMatrixQwords;

 


 


 
typedef struct {
    f32 matrices[0x40][16];          
    f32 point[4];                    
    s32 screen[4];                   
} ModelProjectScratch;



 
typedef struct {
    Vector4 position;                
    Vector4 rotation;                
    Vector4 scale;                   
    u16 unk30;                       
    u16 unk32;                       
    u16 unk34;                       
    u16 unk36;                       
    union {
        u64 flags;
        u8 bytes[8];                 
    } unk38;                         
    u16 unk40;                       
    u16 unk42;                       
    s16 unk44;                       
    u8 unk46;                        
    char pad47[0x01];
    u8 unk48;                        
    char pad49[0x01];
    s16 unk4A;                       
    char pad4C[0x06];
    u16 unk52;                       
    u16 unk54;                       
    u8 unk56;                        
    char pad57[0x01];
    u16 unk58;                       
    u8 unk5A;                        
    char pad5B[0x25];
} MapObjectUnk08;

 
typedef struct {
    char pad00[0x44];
    s16 unk44;                       
    s16 unk46;                       
    s16 unk48;                       
    s16 unk4A;                       
    s16 unk4C;                       
    s16 unk4E;                       
    s16 unk50;                       
    s16 unk52;                       
} MapObjectUnk08Words;



 
typedef struct {
    char pad00[0x44];
    s16 words[8];                    
} MapObjectUnk08Array;



 
typedef struct {
    u8 anim;                         
    u8 sound_kind;                   
    u16 unk02;                       
    u16 sound;                       
    u8 unk06;                        
    u8 sound_frame;                  
} MapObjectAnim;

 
typedef struct {
    u32 kind : 8;                    
    u32 bit8 : 1;                    
    u32 pad00_9 : 2;
    u32 bit11 : 1;                   
    u32 bit12 : 1;                   
    u32 pad00_13 : 2;
    u32 bit15 : 1;                   
    u32 pad00_16 : 16;
    u16 unk04;                       
    u16 unk06;                       
    char pad08[0x04];
    f32 unk0C;                       
    MapObjectAnim anims[8];          
    u16 unk50;                       
    s16 unk52[9];                    
    f32 unk64;                       
    u32 unk68;                       
    f32 unk6C;                       
    f32 unk70;                       
    u8 unk74;                        
} MapObjectUnk04;

 
typedef struct {
    u16 flags;                       
    u16 toughness;                   
} MapObjectDefHalves;



 



 


 
typedef struct {
    char pad00[0x14];
    s32 unk14;                       
    s32 unk18;                       
} AnimObjectBody;

 
typedef struct {
    ModelInstance *model;            
    void *anims;                     
    char pad08[0x08];
    u8 unk10;                        
    u8 mode;                         
    char pad12[0x02];
    char pad14[0x2C];
    AnimObjectBody body;             
} AnimObject;

 


 
typedef struct {
    ModelInstance *model;            
    MapObjectUnk04 *unk04;           
    MapObjectUnk08 *unk08;           
    s32 unk0C;                       
    u32 unk10;                       
    f32 unk14;                       
    f32 unk18;                       
    void *unk1C;                     
    f32 unk20;                       
    f32 unk24;                       
    f32 unk28;                       
    u8 sound;                        
    char pad2D[0x03];
    f32 unk30;                       
    f32 unk34;                       
    f32 unk38;                       
    f32 unk3C;                       
    s32 work[8];                     
    u32 unk60;                       
    char pad64[0x0C];
} MapObject;

 


 
typedef struct {
    Vector4 unk00;                   
    f32 unk10;                       
    f32 unk14;                       
    f32 unk18;                       
    s32 timer;                       
} MapObjectWork;



 
typedef struct {
    f32 near_distance;               
    f32 far_distance;                
    f32 near_alpha;                  
    f32 far_alpha;                   
} MapObjectWorkFade;

 
typedef struct {
    Vector4 step;                    
    s32 frames;                      
    s32 part;                        
    s32 fired;                       
    s32 timer;                       
} MapObjectWorkSlide;

 
typedef struct {
    Vector4 step;                    
    s32 frames;                      
    s32 part;                        
    s32 fired;                       
    s8 timer;                        
} MapObjectWorkKind121DA8;

 
typedef struct {
    u32 bit0 : 1;                    
    u32 bits1 : 31;
} MapObjectUnk60Bits;



 
typedef struct {
    u32 map : 1;                     
    u32 bit1 : 1;
    u32 bit2 : 1;
    u32 bits3 : 5;
    u32 step : 8;                    
    u32 bit16 : 1;                   
    u32 bit17 : 1;
    u32 bit18 : 1;
    u32 bit19 : 1;
    u32 bit20 : 1;
    u32 bit21 : 1;
    u32 bit22 : 1;
    u32 bit23 : 1;
    u32 anim : 8;                    
} MapObjectBits;



 
typedef struct {
    u8 unk00;                        
    u8 step;                         
    u8 unk02;                        
    s8 anim;                         
} MapObjectBytes;



 
typedef union {
    u32 word;
    MapObjectBits bits;
} MapObjectFlags;



 
typedef struct {
    u32 map : 1;
    u32 bit1 : 1;                    
    u32 bit2 : 1;                    
    u32 bit3 : 1;
    u32 bits4 : 28;
} MapObjectBitsLow;



 
typedef struct {
    char pad00[0x32];
    u16 unk32;                       
    char pad34[0x48];
    ModelInstance *model;            
} MapProp;

 
typedef struct ModelNode {
    s32 unk00;                       
    s32 unk04;                       
    s32 unk08;                       
    s32 unk0C;                       
    char pad10[0x04];
    void *unk14;                     
    ModelInstance *model;            
    s16 item;                        
    s16 unk1E;                       
    struct ModelNode *prev;          
    struct ModelNode *next;          
    char pad28[0x14];
} ModelNode;

 
typedef struct {
    Vector4 position;                
    Vector4 rotation;                
    Vector4 scale;                   
    u16 item;                        
    char pad32[0x02];
    u16 unk34;                       
    u8 required_flag;                
    u8 chance;                       
    u8 group;                        
    char pad39[0x06];
    u8 slot_id;                      
} MapItemDef;

 
typedef struct {
    s16 position[3];                 
    s8 rotation[3];                  
    s8 scale[3];                     
    u16 item;                        
    u8 used;                         
    u8 priority;                     
    s32 order;                       
} DroppedItemRecord;

 
typedef struct {
    DroppedItemRecord records[16];   
} DroppedItemRecords;

 
typedef struct ItemNode {
    MapItemDef *def;                 
    DroppedItemRecord *record;       
    s32 unk08;                       
    u32 flags;                       
    char pad10[0x04];
    void *map_record;                
    ModelInstance *model;            
    u16 item;                        
    s16 index;                       
    struct ItemNode *prev;           
    struct ItemNode *next;           
    f32 unk28;                       
    f32 unk2C;                       
    f32 unk30;                       
    char pad34[0x08];
} ItemNode;

 
typedef struct {
    u32 map : 1;                     
    u32 bits1 : 31;
} ItemNodeMapBit;



 
typedef struct {
    char pad00[0x0C];
    u32 map : 1;                     
    u32 bit1 : 1;
    u32 bit2 : 1;                    
    u32 priority : 4;                
    u32 bits7 : 25;
} ItemNodeBits;



 
typedef struct {
    char *model_data;                
    char *texture;                   
} ItemModelSource;

 
typedef struct {
    s32 active;                      
    char pad04[0x04];
    s32 owner_id;                    
    char pad0C[0x04];
    f32 delta_x;                     
    char pad14[0x04];
    f32 delta_y;                     
    char pad1C[0x04];
    f32 delta_z;                     
    char pad24[0x04];
    s32 frames_left;                 
} RotationMotion;

 
typedef struct {
    ModelInstance *model;            
    char pad04[0x0C];
    int flags;                       
} CollisionOwner;

 
typedef struct {
    ModelPartLevels *tables[0x20];   
    u8 limit;                        
    u8 index;                        
    s16 param;                       
    s32 param_shifted;               
    u32 value;                       
    u16 word;                        
    u8 unk8E;                        
    u8 unk8F;                        
} ModelChannel;

 


 
typedef struct {
    u16 has_motion : 1;              
    u16 bits1 : 15;
} ModelUnk100Bits;



 
typedef struct {
    char pad00[0x40];
    f32 x;                           
    f32 y;                           
    f32 z;                           
} ChannelVector;

 
typedef struct {
    char pad00[0xFC];
    ModelChannel *channel;           
    char pad100[0x30];
    ChannelVector *vec;              
} ChannelOwner;

 
typedef struct {
    ChannelOwner *owner;             
    char pad04[0x08];
    u8 *table;                       
    char pad10[0x18];
    u16 enabled;                     
    char pad2A[0x9C];
    u16 flags;                       
    char padC8[0x08];
    u8 old_index;                    
    s8 cur_index;                    
} ChannelActor;




 
typedef struct {
    u8 bits[0x20];                   
    u8 tags[0x10];                   
    u8 values[0x10];                 
} MapFlagBlock;

 
typedef struct {
    char pad00[0x38];
    u64 pad38_0 : 27;
    u64 map : 1;                     
    u64 pad38_28 : 36;
} MapRecordBits;



 
typedef struct {
    u8 red;                          
    u8 green;                        
    u8 blue;                         
    char pad03[0x01];
    s32 unk04[3];                    
} MapFogColor;

 
typedef struct {
    u64 unk00[4];                    
} MapSlotUnk0C;

 
typedef struct {
    f32 unk00;                       
    f32 unk04;                       
    f32 unk08;                       
    f32 unk0C;                       
    s64 unk10;                       
    char pad18[0x08];
} MapSlotUnk0CValues;

 
typedef struct {
    u64 unk00[16];                   
} MapSlotUnk10Words;

 
typedef struct {
    u64 unk00[8];                    
} MapSlotUnk10Half;

 
typedef struct {
    Vector4 unk00[4];                
    Vector4 unk40[4];                
} MapSlotUnk10Vectors;

 
typedef struct {
    f32 directions[4][4];            
    Vector4 colors[4];               
} MapSlotUnk10Lights;

 
typedef struct {
    char pad00[0x60];
    f32 mirror_height;               
} MapDrawStateMirror;

 
typedef struct {
    char pad00[0x60];
    s64 unk60;                       
    s32 unk68;                       
    s32 unk6C;                       
    s32 unk70;                       
    s32 unk74;                       
    s32 unk78;                       
    char pad7C[0x04];
    s64 unk80;                       
    s32 unk88;                       
    char pad8C[0x0E];
    u8 unk9A;                        
    u8 unk9B;                        
    u8 unk9C;                        
    char pad9D[0x03];
    MapSlotUnk0C unkA0;              
    MapSlotUnk10Words unkC0;         
    char pad140[0x140];
    MapFogColor unk280;              
    MapSlotUnk0C unk290[2][0x20];    
    MapSlotUnk10Words unkA90[2][0x20];   
    f32 unk2A90;                     
    f32 unk2A94;                     
    f32 unk2A98;                     
    char pad2A9C[0x04];
} MapDrawState;

 
typedef struct {
    u64 pad00 : 46;
    u32 bit46 : 1;                   
    u64 pad47 : 17;
} MapDrawFlagBits;



 
typedef struct {
    char pad00[0x60];
    u64 pad60 : 32;
    u32 mode : 8;                    
    u32 bit40 : 1;                   
    u32 bit41 : 1;                   
    u32 bit42 : 1;                   
    u32 bit43 : 1;                   
    u32 bit44 : 1;                   
    u32 bit45 : 1;                   
    u32 pad46 : 18;
    s32 unk68;                       
    s32 unk6C;                       
    s32 unk70;                       
    s32 unk74;                       
    s32 unk78;                       
    char pad7C[0x04];
    s64 unk80;                       
    s32 unk88;                       
} MapDrawStateOverlays;

 
typedef struct MapDrawRecord {
    char pad00[0x34];
    u8 unk34;                        
    u8 unk35;                        
    char pad36[0x02];
    u8 unk38;                        
    char pad39[0x03];
    s32 flags;                       
    f32 unk40[4];                    
    f32 unk50[4];                    
    char pad60[0x0C];
    struct MapDrawRecord *next;      
    char pad70[0x0C];
    ModelInstance *model;            
} MapDrawRecord;

 
typedef struct {
    s16 unk00;                       
    s16 unk02;                       
} MapSlotPair;

 
typedef struct {
    u16 base;                        
    u16 format;                      
    u16 width;                       
    u16 height;                      
    u8 step_x;                       
    u8 step_y;                       
    u8 period;                       
    u8 mode;                         
} MapScrollCopy;

 
typedef struct {
    u16 width;                       
    u16 height;                      
    u16 format;                      
    u16 base;                        
    u16 clut_format;                 
    u16 clut_base;                   
    f32 scale_x;                     
    f32 scale_y;                     
    u16 target_width;                
    u16 target_height;               
    u16 target_format;               
    u16 target_base;                 
    u8 red;                          
    u8 green;                        
    u8 blue;                         
    u8 alpha;                        
    u8 blend;                        
    u8 blend_fix;                    
    u8 period;                       
    u8 mode;                         
    s16 step_x;                      
    s16 step_y;                      
} MapScrollTexture;

 
typedef struct {
    u32 a : 2;
    u32 b : 2;
    u32 c : 2;
    u32 d : 2;
    u32 pad8 : 24;
} MapScrollBlendBits;



 
typedef struct {
    u32 *items[7];                   
} MapSlotPackets;

 
typedef struct {
    char pad00[0x20];
    s32 item_models_offset;          
    char pad24[0x18];
    u16 record_count;                
    char pad3E[0x06];
    u16 item_count;                  
    char pad46[0x2A];
    s8 unk70;                        
    u8 unk71;                        
} MapHeader;

 
typedef struct MapSlot {
    u8 unk00;                        
    u8 tag;                          
    u8 unk02;                        
    char pad03[0x01];
    s32 unk04;                       
    MapHeader *header;               
    MapSlotUnk0C *unk0C;             
    void *unk10;                     
    char *records;                   
    MapFlagBlock *flag_blocks;       
    Vector4 *origins;                
    char pad20[0x04];
    char *unk24;                     
    char *unk28;                     
    char pad2C[0x04];
    char *unk30;                     
    struct SoundSpecSource *unk34;   
    char *unk38;                     
    char *unk3C;                     
    MapSlotPackets unk40;            
    s32 unk5C;                       
} MapSlot;

 
typedef struct {
    s32 unk00;                       
    s32 unk04;                       
    s32 unk08;                       
    s32 unk0C;                       
    s32 unk10;                       
    s32 unk14;                       
    s32 unk18;                       
    char pad1C[0x08];
    s32 unk24;                       
    s32 unk28;                       
    u16 unk2C;                       
    char pad2E[0x02];
    u16 unk30;                       
    u16 unk32;                       
    u16 unk34;                       
    u16 unk36;                       
    char pad38[0x02];
    u16 unk3A;                       
    u16 record_count;                
    u16 unk3E;                       
    u16 unk40;                       
    u16 unk42;                       
    u16 unk44;                       
    char pad46[0x06];
    s32 unk4C;                       
    MapFogColor unk50;               
    f32 unk60;                       
    f32 unk64;                       
    f32 unk68;                       
    f32 unk6C;                       
    s8 unk70;                        
    char pad71[0x07];
    u8 unk78;                        
    u8 unk79;                        
    u8 unk7A;                        
    u8 unk7B;                        
} MapSlotUnk08;

 
typedef struct {
    char pad00[0x34];
    u8 unk34;                        
    u8 unk35;                        
    char pad36[0x02];
    u8 unk38;                        
    u8 unk39;                        
    u8 unk3A;                        
    char pad3B[0x01];
    s32 unk3C;                       
    char pad40[0x3C];
    ModelInstance *model;            
} MapRecordInfo;

 
typedef struct {
    f32 x;                           
    f32 y;                           
    f32 z;                           
    u16 map;                         
} MapLoadRequest;

 
typedef struct {
    Vector4 position;                
    s32 step;                        
    MapLoadRequest *request;         
    MapLoadRequest *next_request;    
    s32 file_handle;                 
    union {
        u64 raw;
        struct {
            u64 bit0 : 1;
            u64 bit1 : 1;
            u64 bit2 : 1;
        } f;
    } flags;                         
    s32 timer;                       
    char pad2C[0x04];
} MapLoadState;

 
typedef struct {
    char pad00[0x14];
    char *unk14;                     
    char *unk18;                     
    char *unk1C;                     
    char *unk20;                     
    char pad24[0x10];
    char *unk34;                     
    char pad38[0x04];
    char *unk3C;                     
    char pad40[0x04];
    ModelInstance *unk44;            
    char *unk48;                     
    char pad4C[0x0C];
} MapSlotMemory;

 
typedef struct {
    int unk00[6];                    
    int fired;                       
} MapTriggerShot;

 
typedef struct {
    f32 unk00;                       
    s32 unk04;                       
    s32 unk08;                       
    s32 unk0C;                       
} MapTriggerShotKind121660;

 
typedef struct {
    char pad00[0x44];
    u16 unk44;                       
} TriggerSource;

 
typedef struct {
    ModelInstance *model;            
    u8 *unk04;                       
    char *unk08;                     
    char pad0C[0x04];
    u8 unk10;                        
    u8 type;                         
    char pad12[0x2E];
    MapTriggerShot shot;             
} MapTrigger;



 
typedef union {
    u32 word;                        
    struct {
        u32 unk10 : 8;
        u32 type : 8;
        u32 pad16 : 1;
        u32 flag_20000 : 1;          
        u32 pad18 : 14;
    } bits;
} MapTriggerFlags;




 
typedef struct {
    s32 phase;                       
    s32 period;                      
} MapTriggerShotKind122E88;

 
typedef struct {
    f32 unk00[4];                    
    f32 unk10;                       
} MapTriggerShotKind122BE8;

 
typedef struct {
    u8 map_tag;                      
    u8 trigger_id;                   
    u16 item;                        
} TriggerItemRecord;



 
typedef struct {
    u8 type;                         
    u16 unk01_0 : 4;                 
    u16 use_mode : 4;                
    u16 unk02;                       
    u16 unk04;                       
    u16 unk06;                       
    u16 unk08;                       
    char pad0A[0x04];
    u16 flags;                       
    char pad10[0x10];
} TriggerTypeDef;

 
typedef struct {
    char pad00[0x0E];
    u8 unlisted : 1;                 
    u8 bit1 : 1;
    u8 bits2 : 6;
    char pad0F[0x11];
} TriggerTypeDefBits;



 
typedef struct MapActor {
    u32 flags;                       
    s8 step;                         
    u8 fade_timer;                   
    char pad06[0x02];
    f32 unk08;                       
    u32 serial;                      
    ModelInstance *body;             
    f32 unk14;                       
    struct MapActor *prev;           
    struct MapActor *next;           
    char pad20[0x04];
    u8 unk24;                        
    char pad25[0x07];
} MapActor;

 
typedef struct {
    u16 flags_low;                   
    u16 amount;                      
    char pad04[0x20];
    s32 object_id;                   
    char pad28[0x04];
} MapActorWords;



 
typedef struct {
    u32 in_use : 1;
    u32 in_reach : 1;                
    u32 map : 1;                     
    u32 bits3 : 13;
    u32 amount : 16;
} MapActorBits;




 
typedef struct {
    s16 x;                           
    s16 y;                           
    s16 z;                           
    char radius;                     
    u8 unk07;                        
    u16 unk08;                       
} ObjectMark;

 
typedef struct {
    ObjectMark marks[16];
} ObjectMarkRow;



 









 
typedef struct {
    f32 spawn_x;                     
    f32 spawn_y;                     
    f32 spawn_z;                     
    char pad0C[0x04];
    f32 unk10;                       
    f32 unk14;                       
    char pad18[0x08];
    f32 unk20[4];                    
    u16 type_id;                     
    char pad32[0x02];
    s8 pool_size;                    
    u8 unk35;                        
    u16 unk36;                       
    u64 unk38;                       
    u8 unk40;                        
    u8 variant;                      
    u8 unk42;                        
    u8 unk43;                        
    u8 unk44;                        
    char pad45[0x03];
    u16 vertical_despawn_range;      
    char pad4A[0x02];
    u16 unk4C;                       
    u16 unk4E;                       
    u16 unk50;                       
    u8 unk52;                        
    char pad53;
    u16 unk54;                       
    u16 unk56;                       
    u16 unk58;                       
    u16 despawn_range;               
} EntitySpawnInfo;

 
typedef union {
    u64 word;
    u32 halves[2];
    u8 bytes[8];                     
} EntitySpawnFlags;



struct Entity;

 
typedef struct {
    char pad00[0x81];
    u8 unk81;                        
    s16 unk82;                       
    s32 unk84;                       
    s32 unk88;                       
    u16 unk8C;                       
} EntityModelUnkFC;

 
typedef struct {
    char pad00[0x50];
    f32 unk50[4];                    
    char pad60[0x08];
    u64 unk68;                       
    char pad70[0x90];
} EntityModelUnkF8;

 
typedef struct {
    char pad00[0x50];
    f32 unk50[4];                    
    char pad60[0x08];
    u64 unk68;                       
    char pad70[0x10];
} EntityModelPart80;

 
typedef struct {
    char pad00[0x6C];
    s8 unk6C;                        
    char pad6D[0x13];
} EntityModelPart80Byte;

 
typedef struct {
    u64 pad0 : 40;
    u64 bit40 : 1;
    u64 bit41 : 1;
    u64 pad42 : 22;
} EntityModelPartFlagBits;



 
typedef struct {
    char pad00[0xC0];
    f32 position[4];                 
    f32 unkD0[4];                    
    f32 unkE0[4];                    
    char padF0[0x08];
    EntityModelUnkF8 *unkF8;         
    EntityModelUnkFC *unkFC;         
    char pad100[0x02];
    u16 unk102;                      
    char pad104[0x30];
    s32 unk134;                      
    void *unk138;                    
    f32 unk13C;                      
    char pad140[0x1C];
    f32 unk15C;                      
} EntityModel;

 
typedef struct {
    s16 unk00[9];                    
} EntityParamsUnk240PartUnk00;

typedef struct {
    s16 unk00[6];                    
} EntityParamsUnk240PartUnk12;

 
typedef struct {
    EntityParamsUnk240PartUnk00 unk00;  
    EntityParamsUnk240PartUnk12 unk12;  
    u8 unk1E;                        
    u8 unk1F;                        
    u16 unk20;                       
    u16 unk22;                       
    u16 unk24;                       
    u16 unk26;                       
} EntityParamsUnk240Part;

 
typedef struct {
    u16 flags;                       
} EntityParamsUnk240PartFlags;



 
typedef struct {
    u8 unk00;                        
    u8 unk01;                        
    u8 unk02;                        
    u8 unk03;                        
    u16 unk04;                       
    u16 unk06;                       
    f32 unk08;                       
    u8 unk0C;                        
    char pad0D;
    s16 unk0E;                       
    EntityParamsUnk240Part unk10[3];  
} EntityParamsUnk240;

 
typedef struct {
    f32 position[4];                 
    f32 unk10[4];                    
    f32 unk20[4];                    
    f32 unk30;                       
    f32 unk34;                       
    f32 unk38;                       
    f32 unk3C;                       
    f32 unk40;                       
    f32 unk44;                       
    struct Entity *owner;            
    u64 unk4C_0 : 2;                 
    u64 unk4C_2 : 1;
    u64 unk4C_3 : 1;
    u64 unk4C_4 : 4;
    u8 unk4D;                        
    u8 unk4E;                        
    u8 unk4F;                        
    EntityParamsUnk240PartUnk00 unk50;  
    EntityParamsUnk240PartUnk12 unk62;  
    u8 unk6E;                        
    u8 unk6F;                        
    u8 unk70;                        
    u8 unk71;                        
    u16 unk72;                       
    u8 unk74;                        
    char pad75[0x03];
    f32 unk78;                       
    f32 unk7C;                       
} Unk1B1470Request;

typedef struct {
    u8 unk00;                        
    char pad01;
    s16 unk02;                       
    s32 unk04;                       
    char pad08[0x06];
    s16 unk0E[6];                    
    char pad1A[0x02];
    s32 unk1C;                       
    s16 unk20;                       
    s16 unk22;                       
    s16 unk24;                       
    s16 unk26;                       
    char pad28[0x08];
} Unk1B1470Extra;

 
typedef struct {
    char pad00[0x4C];
    u8 unk4C_0 : 3;                  
    u8 unk4C_3 : 1;
    u8 unk4C_4 : 4;
    char pad4D[0x33];
} __attribute__((aligned(8))) Unk1B1470RequestBits;

 
typedef struct {
    char pad00[0x48];
    struct Entity *owner;            
    u64 flags : 32;                  
} Unk1B1470RequestWord;

 
typedef struct {
    char pad00[0x48];
    u64 owner_and_flags;             
} Unk1B1470RequestUnit;


 


 
typedef struct {
    s64 value;
} __attribute__((packed)) EntityUnaligned64;

 
typedef struct {
    char unk00;                      
    char pad01[0x1F];
    u16 unk20;                       
    s16 unk22;                       
    char pad24[0x04];
    EntityUnaligned64 unk28;         
} EntityUnk1B1390;

 
typedef struct {
    u8 unk00;                        
    u8 unk01;                        
    u16 unk02;                       
} EntityParamsUnk3D8Launch;

 
typedef struct {
    u8 unk00;                        
    u8 unk01;                        
    u8 unk02;                        
    u8 unk03;                        
    char pad04[0x08];
    u8 unk0C;                        
    char pad0D[0x03];
    EntityParamsUnk3D8Launch unk10[3];  
} EntityParamsUnk3D8Full;

 
typedef struct {
    u8 unk00;                        
    u8 unk01;                        
    u8 unk02;                        
    u8 unk03;                        
} EntityParamsUnk3D8;

typedef struct {
    EntityParamsUnk3D8 record;       
    u16 reach_min;                   
    u16 reach_max;                   
    f32 unk08;                       
    u8 unk0C;                        
    char pad0D;
    s16 unk0E;                       
    u8 unk10;                        
    u8 unk11;                        
    char pad12[0x0A];
} EntityParamsUnk3D8Array;

 
 
typedef struct {
    u8 index : 5;                    
    u8 moves_entity : 1;             
    u8 bit6 : 1;
    u8 backwards : 1;                
    u8 unk01;                        
    s16 frame;                       
    s32 frame_fixed;                 
} EntityMotionSlot;




typedef struct {
    union {
        u32 flags;                   
        u8 point;                    
    } unk00;
    u16 unk04;                       
    u16 unk06;                       
    f32 unk08;                       
} EntityBodyPoint;

 
typedef struct {
    u32 unk00_0 : 16;                
    u32 unk02 : 8;                   
    u32 unk03_0 : 1;                 
    u32 unk03_1 : 2;
    u32 unk03_3 : 5;
    u16 unk04;                       
    u16 unk06;                       
    u16 unk08;                       
    u16 unk0A;                       
    u16 unk0C;                       
    u16 unk0E;                       
    u16 flags;                       
    char pad12[0x02];
    u8 unk14[4];                     
    u16 unk18;                       
    u16 unk1A;                       
    char pad1C[0x04];
    EntityBodyPoint unk20[4];        
    f32 unk50;                       
    f32 unk54;                       
    f32 unk58;                       
    f32 turn_limit;                  
    u8 unk60[0x20];                  
    u16 unk80[0x22];                 
    u8 unkC4;                        
    char padC5;
    u16 unkC6;                       
    u8 unkC8;                        
    u8 unkC9;                        
    u8 unkCA;                        
    u8 unkCB;                        
    u16 unkCC;                       
    u16 unkCE;                       
    u8 unkD0;                        
    u8 unkD1;                        
    u8 unkD2;                        
    char padD3;
    s16 unkD4;                       
    s16 unkD6;                       
    s16 unkD8;                       
    s16 unkDA;                       
    s16 unkDC;                       
    s16 unkDE;                       
    s16 unkE0;                       
    s16 unkE2;                       
    s16 unkE4;                       
    u16 unkE6;                       
    u16 unkE8;                       
    u8 unkEA;                        
    u8 unkEB;                        
    u8 unkEC;                        
    u8 unkED;                        
    u16 unkEE;                       
    u8 unkF0[0x20];                  
    u8 unk110[0x20];                 
    s32 unk130[0x20];                
    s32 unk1B0[0x20];                
    u16 gold;                        
    u16 drop_item;                   
    u16 second_drop_item;            
    s8 drop_chance;                  
    s8 second_drop_chance;           
    EntityUnaligned64 unk238;        
    EntityParamsUnk240 unk240[2];    
    char unk350[0x88];               
    EntityParamsUnk3D8Array unk3D8[3];  
    u8 unk42C;                       
    u8 unk42D;                       
    u16 unk42E;                      
    s16 unk430;                      
    s16 unk432;                      
    u16 unk434;                      
    s16 unk436;                      
    s16 unk438;                      
    s16 unk43A;                      
    s16 unk43C;                      
    s16 unk43E;                      
    u16 unk440[8];                   
} EntityParams;

 


 
typedef struct {
    u16 unk00;                       
    u16 unk02;                       
    u8 unk04;                        
    s8 unk05;                        
} EntityParamsUnk130;

 
typedef struct {
    u8 unk00;                        
    u8 unk01;                        
    u16 unk02;                       
    u8 unk04;                        
    char pad05[0x03];
} EntityParamsUnk1B0;

 

 



 
typedef struct {
    char pad00[0x3D8];
    EntityParamsUnk3D8Array ranged[3];  
} EntityParamsRangedView;



 
typedef struct {
    char pad00[0xC0];
    f32 motion_scale;                
} EntityParamsMotionScale;



 
typedef struct {
    u8 unk00;                        
    u8 unk01;                        
    u8 unk02;                        
    u8 unk03;                        
} EntityParamsBytes;

 
typedef struct {
    u32 unk00;                       
} EntityParamsWord;

 
typedef struct {
    void (*unk00)(struct Entity *entity, s32 unknown);  
    void (*unk04)(void);             
    void (*unk08)(void);             
    void (*unk0C)(void);             
    void (*unk10)(void);             
    void (*unk14)(void);             
    void (*unk18)(void);             
    f32 unk1C;                       
    f32 unk20[4];                    
    f32 unk30[4];                    
    f32 unk40;                       
    f32 unk44;                       
    f32 unk48;                       
    f32 unk4C;                       
    f32 unk50;                       
    f32 unk54;                       
    f32 unk58;                       
    f32 unk5C;                       
    f32 unk60;                       
    char pad64[0x04];
    s16 unk68;                       
    s16 unk6A;                       
    s16 unk6C;                       
    s16 unk6E;                       
    char pad70[0x08];
    s8 unk78;                        
    s8 unk79;                        
    char unk7A;                      
    char unk7B;                      
    char unk7C;                      
    char pad7D[0x03];
    char unk80[0x08];                
    char unk88[0x08];                
} EntityUnkE0;

 
typedef struct {
    u8 unk00;                        
    u8 unk01;                        
    s16 unk02;                       
    s32 unk04;                       
} EntityUnkE0Slot;

 
typedef struct {
    void (*unk00)(struct Entity *entity, s32 unknown);  
    char pad04[0x14];
    void (*unk18)(void);             
    s32 unk1C;                       
    char pad20[0x10];
    f32 unk30[4];                    
    char pad40[0x08];
    f32 unk48;                       
    f32 unk4C;                       
    f32 unk50;                       
    char pad54[0x14];
    s16 unk68;                       
    char pad6A[0x0E];
    s8 unk78;                        
    s8 unk79;                        
    s8 unk7A;                        
    char pad7B[0x05];
    EntityUnkE0Slot unk80;           
    EntityUnkE0Slot unk88;           
} EntityUnkE0Kind1740A8;

 
typedef struct {
    void (*unk00)(struct Entity *entity, s32 unknown);  
    char pad04[0x14];
    void (*unk18)(void);             
    f32 unk1C;                       
    char pad20[0x24];
    f32 unk44;                       
    f32 unk48;                       
    f32 unk4C;                       
    char pad50[0x28];
    s8 unk78;                        
    s8 unk79;                        
    char pad7A[0x06];
    EntityUnkE0Slot unk80;           
    EntityUnkE0Slot unk88;           
    char pad90[0x10];
} EntityUnkE0Kind1719F8;

 
typedef struct {
    void (*unk00)(struct Entity *entity, s32 unknown);  
    char pad04[0x14];
    void (*unk18)(void);             
    s32 unk1C;                       
    f32 unk20[4];                    
    f32 unk30[4];                    
    char pad40[0x3A];
    s8 unk7A;                        
    s8 unk7B;                        
    s8 unk7C;                        
    char pad7D[0x23];
} EntityUnkE0Kind178358;

 
typedef struct {
    char pad00[0x1C];
    s32 unk1C;                       
} EntityUnkE0Kind150588;

 
typedef struct {
    void (*unk00)(struct Entity *entity, s32 unknown);  
    void (*unk04)(void);             
    void (*unk08)(void);             
    char pad0C[0x04];
    void (*unk10)(void);             
    void (*unk14)(void);             
    void (*unk18)(void);             
    f32 unk1C;                       
    f32 unk20[4];                    
    char pad30[0x10];
    f32 unk40;                       
    f32 unk44;                       
    char pad48[0x22];
    s16 unk6A;                       
    s16 unk6C;                       
    s16 unk6E;                       
    char pad70[0x08];
    s8 unk78;                        
    s8 unk79;                        
    s8 unk7A;                        
    s8 unk7B;                        
    s8 unk7C;                        
    char pad7D[0x23];
} __attribute__((aligned(8))) EntityUnkE0Kind168160;

 
typedef struct {
    u8 index : 5;                    
    u64 bit5 : 1;
    u64 bit6 : 1;
    u64 pad7 : 1;
    u8 unk01;                        
    s16 unk02;                       
    s32 unk04;                       
} EntityUnkE0SlotBits;

 
typedef struct {
    char pad00[0x80];
    EntityUnkE0SlotBits unk80;       
    EntityUnkE0SlotBits unk88;       
} EntityUnkE0Kind151D30;

 
typedef struct {
    char pad00[0x1C];
    f32 unk1C;                       
    char pad20[0x24];
    f32 unk44;                       
    f32 unk48;                       
    f32 unk4C;                       
    char pad50[0x50];
} EntityUnkE0Kind16BF98;

 
typedef struct {
    char pad00[0x68];
    s16 unk68;                       
    s16 unk6A;                       
    s16 unk6C;                       
    char pad6E[0x0A];
    s8 unk78;                        
    s8 unk79;                        
    s8 unk7A;                        
    s8 unk7B;                        
    s8 unk7C;                        
    s8 unk7D;                        
    s8 unk7E;                        
    char pad7F[0x21];
} __attribute__((aligned(8))) EntityUnkE0Kind15F8C0;

 
typedef struct {
    char pad00[0x30];
    f32 unk30[4];                    
    char pad40[0x2E];
    s16 unk6E;                       
    char pad70[0x10];
    f32 unk80[4];                    
    f32 unk90[4];                    
} EntityUnkE0Kind162BC8;

 
typedef struct {
    char pad00[0x68];
    s16 unk68[3];                    
    char pad6E[0x32];
} EntityUnkE0Kind17FC60;

 
typedef struct {
    void (*unk00)(struct Entity *entity, s32 unknown);  
    char pad04[0x14];
    void (*unk18)(void);             
    f32 unk1C;                       
    char pad20[0x4A];
    s16 unk6A;                       
    char pad6C[0x0E];
    s8 unk7A;                        
    s8 unk7B;                        
    s8 unk7C;                        
    s8 unk7D;                        
    s8 unk7E;                        
    char pad7F[0x21];
} EntityUnkE0Kind131390;

 
typedef struct {
    char pad00[0x1C];
    f32 unk1C;                       
    char pad20[0x20];
    f32 unk40[7];                    
    char pad5C[0x44];
} EntityUnkE0Kind12ACA0;

 
typedef struct {
    char pad00[0x10];
    u16 flags;                       
    u16 unk12;                       
    char pad14[0x04];
    u16 unk18;                       
    u16 unk1A;                       
    f32 unk1C;                       
    char pad20[0x3C];
    f32 turn_limit;                  
    char pad60[0x64];
    u8 unkC4;                        
    u8 unkC5;                        
    u16 unkC6;                       
    char padC8[0x368];
    s16 unk430;                      
    char pad432[0x02];
    s16 unk434;                      
    char pad436[0x02];
    s16 unk438;                      
    s16 unk43A;                      
} EntityController;

 


typedef struct Entity {
    EntityModel *model;              
    char pad04[0x08];
    EntityParams *params;            
    EntitySpawnInfo *spawn_info;     
    void *unk14;                     
    struct Entity *prev;             
    struct Entity *next;             
    u16 unk20;                       
    char pad22[0x02];
    u16 unk24;                       
    u16 unk26;                       
    u16 unk28;                       
    u16 unk2A;                       
    u16 unk2C;                       
    u16 unk2E;                       
    f32 unk30;                       
    f32 unk34;                       
    f32 radius;                      
    f32 height;                      
    f32 unk40;                       
    f32 unk44;                       
    f32 unk48;                       
    f32 unk4C;                       
    s32 unk50;                       
    void (*handler)(struct Entity *entity);       
    void (*prev_handler)(struct Entity *entity);  
    void *target;                    
    f32 unk60;                       
    f32 distance_to_player;          
    f32 unk68;                       
    f32 unk6C;                       
    f32 unk70[4];                    
    u16 unk80;                       
    u16 unk82;                       
    f32 unk84;                       
    f32 unk88;                       
    f32 unk8C;                       
    f32 unk90;                       
    f32 unk94;                       
    f32 unk98;                       
    u16 timer;                       
    u16 unk9E;                       
    u16 unkA0;                       
    u8 unkA2;                        
    u8 unkA3;                        
    f32 unkA4[3];                    
    f32 unkB0[3];                    
    u8 unkBC[4];                     
    u8 unkC0[6];                     
    u16 unkC6;                       
    u16 unkC8;                       
    s8 action_step;                  
    s8 unkCB;                        
    s8 state;                        
    s8 unkCD;                        
    char prev_state;                 
    s8 unkCF;                        
    char padD0;
    s8 unkD1;                        
    char unkD2;                      
    char unkD3;                      
    char unkD4;                      
    char group_id;                   
    u8 unkD6;                        
    u8 unkD7;                        
    u64 flags;                       
    EntityUnkE0 unkE0;               
    char pad170[0x10];               
} Entity;

 


typedef struct {
    char pad00[0xE0];
    u8 work_bytes[0xA0];    
} EntityWorkBytes;



 
typedef struct {
    u32 bit0 : 1;
    u32 bit1 : 1;
    u32 bits2 : 2;
    u32 bit4 : 1;
    u32 bit5 : 1;
    u32 bit6 : 1;
    u32 bit7 : 1;
    u32 pad8 : 1;
    u32 bit9 : 1;
    u32 bit10 : 1;
    u32 bits11 : 3;
    u32 bit14 : 1;
    u32 bit15 : 1;
    u32 bit16 : 1;
    u32 bit17 : 1;
    u32 bit18 : 1;
    u32 bit19 : 1;
    u32 bit20 : 1;
    u32 bit21 : 1;
    u32 bit22 : 1;
    u32 bit23 : 1;
    u32 bit24 : 1;
    u32 bit25 : 1;
    u32 bit26 : 1;
    u32 bit27 : 1;
    u32 bit28 : 1;
    u32 bit29 : 1;
    u32 bit30 : 1;
    u32 bit31 : 1;
    u32 bit32 : 1;
    u32 bit33 : 1;
    u32 bit34 : 1;
    u32 bit35 : 1;
    u32 bits36 : 2;
    u32 bit38 : 1;
    u32 bit39 : 1;
    u32 pad40 : 1;
    u32 bit41 : 1;
    u32 bit42 : 1;
    u32 bit43 : 1;
    u32 bit44 : 1;
    u32 bit45 : 1;
    u32 bit46 : 1;
    u32 bit47 : 1;
    u32 bit48 : 1;
    u32 bit49 : 1;
    u32 bit50 : 1;
    u32 bit51 : 1;
    u32 bit52 : 1;
    u32 bit53 : 1;
    u32 bit54 : 1;
    u32 bit55 : 1;
    u32 bit56 : 1;
    u64 bit57 : 1;
    u32 bits58 : 2;
    u32 bits60 : 3;
    u64 pad63 : 1;
} EntityFlagBits;



 
typedef struct {
    u64 pad0 : 36;
    u32 hit_strength : 2;            
    u32 hit_source : 2;              
    u64 pad40 : 24;
} EntityFlagHitBits;



 
typedef union {
    s32 as_s32;
    f32 as_f32;
} EntityUnk5C;



 
typedef struct {
    u32 pad0 : 25;
    u32 bit25 : 1;
    u32 bit26 : 1;
    u32 pad27 : 5;
} EntityParamsBits;



 
typedef union {
    u64 word;
    u8 bytes[8];
} EntityFlagWord;



 
typedef struct {
    f32 position[4];                 
    char pad10[0x04];
    f32 unk14;                       
    char pad18[0x1EC];
    Entity *unk204;                  
    char pad208[0x4A];
    u16 unk252;                      
    char pad254[0x05];
    u8 unk259;                       
} PlayerData;

 
typedef struct {
    s32 frames[4];                   
} EntityFrameTable;

 
typedef struct {
    s32 weights[10];                 
} EntityRangeWeights;

 
typedef struct {
    s32 frames[5];                   
} EntityFrameTable5;

 
typedef struct {
    s32 ids[5];                      
} EntityIdTable;

 
typedef struct {
    s32 weights[3];                  
} EntityActionWeights3;

 
typedef struct {
    f32 x;                           
    f32 y;                           
    f32 z;                           
    f32 w;                           
} KfVector;

 
typedef struct {
    char pad00[0x1C];
    f32 unk1C;                       
    char pad20[0x0C];
    u16 unk2C;                       
    u16 unk2E;                       
    char pad30[0x02];
    u16 unk32;                       
    char pad34[0x02];
    u8 unk36;                        
    u8 unk37;                        
    u64 unk38;                       
    char pad40[0x0E];
    u8 unk4E;                        
    char pad4F[0x11];
    f32 unk60[4];                    
    f32 unk70[4];                    
} MapRecord;

 
typedef struct {
    char pad00[0x04];
    f32 unk04;                       
    char pad08[0x0C];
    f32 unk14;                       
    char pad18[0x0C];
    f32 unk24;                       
    char pad28[0x28];
    f32 unk50;                       
    f32 unk54;                       
    f32 unk58;                       
    char pad5C[0x14];
    f32 unk70;                       
    f32 unk74;                       
    f32 unk78;                       
    char pad7C[0x04];
} MapSlotUnk10;

 
typedef union {
    u64 all;
    u32 halves[2];                   
} EntityFlags;


 
typedef union {
    u16 all;
} EntityCommand;


 
typedef struct {
    char pad00[0x0E];
    s16 degrees;                     
} TurnCommand;

 
typedef struct {
    char unk00;                      
    char unk01;                      
    char unk02;                      
    char pad03[0x3D];
} EntityGroupRecord;



 








 



 
typedef struct {
    union {
        u32 word;                    
        struct {
            u8 kind;                 
            u16 unk2;                
        } fields;
        struct {
            u32 kind : 8;
            u32 flag_100 : 1;        
            u32 flag_200 : 1;        
            u32 pad10 : 6;
            u32 unk2 : 16;
        } bits;
    } head;
    s16 unk4;                        
    u8 unk6;                         
    u8 unk7;                         
    int age;                         
    int base_address;                
} EntityPoolSlot;

extern EntityPoolSlot D_003D6DF0[5];

 
typedef struct {
    char pad00[0x14];
    int unk14;                       
    int unk18;                       
    char pad1C[0x04];
} EntityTypeFileHeader;

 
typedef struct {
    char pad00[0x04];
    char *model_offset;              
    char *motion_offset;             
    s32 unk0C;                       
    s32 pool_model_offset;           
} EntityPoolData;

 
typedef struct LinkedPoint {
    Vector4 position;                
    Vector4 target;                  
    Vector4 *follow;                 
    f32 progress_scale;              
    f32 duration;                    
    u16 handle;                      
    u8 active;                       
    u8 list_index;                   
    u8 kind_a;                       
    s8 kind_b;                       
    s16 unk32;                       
    char pad34[0x04];
    struct LinkedPoint *prev;        
    struct LinkedPoint *next;        
} LinkedPoint;



extern LinkedPoint D_003E7500[0x200];

 



 
typedef struct {
    ModelInstance model;
} __attribute__((aligned(8))) ModelInstanceBlock;

extern Entity *D_004467E0;           

float func_00124DE0(Entity *entity);     

 
typedef struct {
    char pad00[0x20];
    s64 flags;                       
    Entity *owner;                   
    struct EffectActor *actor;       
    char pad30[0x08];
    s32 unk38;                       
    u16 object_id;                   
    char pad3E[0x12];
} ObjectSlot;

 
typedef struct {
    u64 pad0 : 32;
    u64 bit32 : 1;
    u64 bit33 : 1;
    u64 pad34 : 30;
} ObjectSlotFlagBits;



 
typedef struct {
    char pad00[0x30];
    u16 unk30;                       
    char pad32[0x1E];
    s16 unk50;                       
    s16 unk52;                       
} ReferencedObject;

 
typedef struct {
    ModelInstance *model;            
    char pad04[0x04];
    ReferencedObject *unk08;         
    char pad0C[0x04];
    s32 unk10;                       
} ObjectReference;

void Entity_ResetAllLists(void);
void Entity_FreeAllInGroup(int group_id);
void Entity_DespawnOutOfRange(void);



 






 
typedef struct {
    u16 values[6];                   
} PlayerItemRates;

 
typedef struct {
    s16 values[9];                   
} PlayerShotStats;

 
typedef struct {
    u8 unk00;                        
    char pad01[0x01];
    u16 unk02;                       
    void *unk04;                     
    PlayerShotStats unk08;           
    char pad1A[0x06];
    u16 unk20;                       
    u16 unk22;                       
    s16 unk24;                       
    u16 unk26;                       
    char pad28[0x08];
} PlayerShot;

 
typedef struct {
    Vector4 position;                
    Vector4 rotation;                
    s32 unk20;                       
    s32 unk24;                       
    s32 unk28;                       
    s32 unk2C;                       
    s16 timed_flags[6];              
    u8 unk3C;                        
    u8 unk3D;                        
    u8 equipment[8];                 
    u16 unk46;                       
    u8 unk48;                        
    char pad49[0x03];
    s32 unk4C;                       
    s32 unk50;                       
    s32 unk54;                       
    s32 unk58;                       
    s32 unk5C;                       
    PlayerShotStats unk60;           
    PlayerItemRates unk72;           
    PlayerShotStats unk7E;           
    PlayerShotStats unk90;           
    char padA2[0x02];
    s32 unkA4;                       
    s32 unkA8;                       
    s32 unkAC;                       
    s32 unkB0;                       
    s32 unkB4;                       
    s32 unkB8;                       
    s32 unkBC;                       
    s32 unkC0;                       
    s32 unkC4;                       
    s32 unkC8;                       
    s32 unkCC;                       
    s32 unkD0;                       
    s32 unkD4;                       
    s32 unkD8;                       
    u16 unkDC[6];                    
    s32 unkE8;                       
    s32 unkEC;                       
    u32 unkF0;                       
    u32 unkF4;                       
    u32 unkF8;                       
    u32 unkFC;                       
    s32 unk100;                      
    s32 unk104;                      
    s32 unk108;                      
    s32 unk10C;                      
    u16 unk110[6];                   
    s16 unk11C[3];                   
    s16 unk122[6];                   
    u16 unk12E[6];                   
    s16 unk13A[3];                   
    s16 unk140[6];                   
    u16 unk14C[6];                   
    s16 unk158[3];                   
    s16 unk15E[6];                   
    u16 unk16A[6];                   
    s16 unk176[3];                   
    s16 unk17C[6];                   
    s16 unk188[3];                   
    s16 unk18E[6];                   
    char pad19A[0x02];
    s32 unk19C;                      
    s32 unk1A0;                      
    s16 unk1A4;                      
    s16 unk1A6;                      
    u8 unk1A8;                       
    char pad1A9[0x03];
    f32 unk1AC;                      
    f32 unk1B0;                      
    f32 unk1B4;                      
    f32 unk1B8;                      
    f32 unk1BC;                      
    s32 unk1C0;                      
    f32 unk1C4;                      
    u64 unk1C8;                      
    f32 unk1D0;                      
    f32 unk1D4;                      
    f32 unk1D8;                      
    f32 unk1DC;                      
    s32 unk1E0;                      
    s32 unk1E4;                      
    s32 unk1E8;                      
    s32 unk1EC;                      
    f32 unk1F0;                      
    f32 unk1F4;                      
    s32 unk1F8;                      
    s32 unk1FC;                      
    s32 unk200;                      
    s32 unk204;                      
    s32 unk208;                      
    s32 unk20C;                      
    u8 unk210;                       
    u8 unk211;                       
    char pad212[0x02];
    f32 unk214;                      
    char pad218[0x08];
    Vector4 aim;                     
    f32 aim_speed;                   
    s32 unk234;                      
    f32 unk238;                      
    f32 unk23C;                      
    Vector4 unk240;                  
    u8 unk250;                       
    char pad251[0x01];
    u16 map_index;                   
    u32 unk254;                      
    u8 unk258;                       
    u8 unk259;                       
    u8 unk25A;                       
    u8 unk25B;                       
    u8 unk25C;                       
    u8 unk25D;                       
    u8 unk25E;                       
    u8 unk25F;                       
    u8 unk260_0 : 1;                 
    u8 unk260_1 : 1;                 
    u8 unk260_2 : 1;                 
    u8 unk260_3 : 5;
    u8 unk261;                       
    u8 unk262;                       
    u8 unk263;                       
    u8 unk264;                       
    char pad265[0x03];
    PlayerShot unk268;               
    u8 unk298;                       
    char pad299[0x03];
    s32 unk29C;                      
    s32 unk2A0;                      
    char pad2A4[0x04];
    f32 unk2A8;                      
    char pad2AC[0x04];
    s32 unk2B0;                      
    s32 unk2B4;                      
    u16 flag_timers[6];              
    u32 unk2C4;                      
    u32 unk2C8;                      
    u32 unk2CC;                      
    u32 unk2D0;                      
    u32 unk2D4;                      
    u32 unk2D8;                      
    s32 unk2DC;                      
    s32 unk2E0;                      
    s32 unk2E4;                      
    f32 unk2E8;                      
    s32 unk2EC;                      
    s32 unk2F0;                      
    char pad2F4[0x0C];               
} PlayerState;

 
typedef struct {
    s32 red;                         
    s32 green;                       
    s32 blue;                        
    s32 alpha;                       
    s64 blend;                       
    s32 lifetime;                    
    char pad1C[0x04];
} WeaponTrailKind;

 
typedef struct {
    s32 unk00;                       
    s32 unk04;                       
    s32 unk08;                       
    s32 unk0C;                       
    s32 unk10;                       
    s32 unk14;                       
    s32 unk18;                       
    s32 red;                         
    s32 green;                       
    s32 blue;                        
    s32 unk28;                       
    s32 kind;                        
    s32 unk30;                       
    s32 unk34;                       
    char pad38[0x10];
} PlayerScreenTint;

 
typedef struct {
    u64 pad0 : 33;
    u64 bit33 : 1;
    u64 bit34 : 1;                   
    u64 bit35 : 1;                   
    u64 bit36 : 1;                   
    u64 pad37 : 1;
    u64 bit38 : 1;                   
    u64 bit39 : 1;
    u64 pad40 : 24;
} PlayerStatusBits;

 
typedef struct {
    char pad000[0x1C8];
    s32 step_sounded;                
    u64 pad32 : 1;                   
    u64 bit33 : 1;
    u64 bit34 : 1;
    u64 bit35 : 1;
    u64 bit36 : 1;
    u64 pad37 : 1;
    u64 bit38 : 1;
    u64 bit39 : 1;
    u64 pad40 : 24;
} PlayerStatusView;





 
typedef struct {
    s32 experience;                  
    u8 unk04;                        
    u8 unk05;                        
    u8 unk06;                        
    u8 unk07;                        
} PlayerLevelStep;

 
typedef struct {
    s16 unk00[9];                    
    u16 unk12[6];                    
    char pad1E[0x01];
    u8 bonus_kinds[2];               
    u8 bonus_amounts[2];             
    char pad23[0x1C];
    u8 unk3F;                        
} LevelRecordDef;

 
typedef struct {
    s16 unk00[9];                    
    u8 level;                        
    u8 exp;                          
    u8 unk14;                        
    char pad15[0x01];
    s16 unk16;                       
    u8 unk18;                        
    char unk19[8];                   
    char pad21[0x01];
} LevelRecord;

 
typedef struct {
    PlayerShotStats unk00[3];        
    PlayerItemRates unk36;           
    u16 unk42;                       
    u16 unk44;                       
    u16 unk46;                       
    u16 unk48;                       
    u16 unk4A;                       
    u16 unk4C;                       
    u16 unk4E;                       
    char pad50[0x02];
    u16 unk52;                       
    u16 unk54;                       
    u16 unk56;                       
    u16 unk58;                       
    u16 unk5A;                       
    u16 unk5C;                       
    u16 unk5E;                       
    char pad60[0x02];
    u8 unk62;                        
    u8 unk63;                        
    u8 bonus_kinds[2];               
    u8 bonus_amounts[2];             
    u16 unk68;                       
    char pad6A[0x02];
    f32 unk6C;                       
    f32 unk70;                       
    f32 unk74;                       
    u16 unk78;                       
    u16 unk7A;                       
    u8 unk7C;                        
    u8 unk7D;                        
    u8 unk7E;                        
    u8 unk7F;                        
    u16 unk80;                       
    u16 unk82;                       
    char pad84[0x02];
    u8 unk86;                        
    s8 unk87;                        
    s8 unk88;                        
    s8 unk89;                        
    u16 unk8A;                       
    u16 unk8C;                       
    char pad8E[0x02];
} PlayerItemDef;

 
typedef struct {
    VuVector4 points[2];
} PlayerArmPoints;

 
typedef struct {
    s32 texture_size;                
    s32 model_size;                  
    s32 motion_size;                 
    char pad0C[0x08];
    s32 texture_offset;              
    s32 model_offset;                
    s32 motion_offset;               
} PlayerArmFile;

 
typedef struct {
    char pad00[0x30];
    u32 unk30_0 : 18;                
    u32 state : 2;                   
    u32 unk30_20 : 12;
    char pad34[0x02];
    u8 unk36;                        
    u8 exp_per_level;                
    char pad38[0x04];
    u8 unk3C;                        
    char pad3D[0x01];
    u16 unk3E;                       
} LevelDef;

 
typedef struct {
    char key[0x40];                  
    void (*handler)(char *);         
} IniKeyHandler;



 





 
typedef struct {
    char unk00;                      
    char unk01;                      
    char paused;                     
    s8 unk03;                        
    u8 unk04;                        
    s8 unk05;                        
    char unk06;                      
    char unk07;                      
    char pad08[0x18];
    int unk20;                       
    u32 bit0 : 1;                    
    u32 bit1 : 1;                    
    u32 bit2 : 1;
    u32 bit3 : 1;
    u32 bit4 : 1;
    u32 bit5 : 1;
    u32 bit6 : 1;
    u32 bit7 : 1;
    u32 bit8 : 1;
    u32 bit9 : 1;                    
    u32 bits10 : 2;
    u32 bit12 : 1;                   
    u32 bit13 : 1;
    u32 bit14 : 1;
    int unk28;                       
    int unk2C;                       
    int unk30;                       
    int unk34;                       
    char pad38[0x08];
    void *unk40;                     
    int unk44;                       
} GameStatus;

 
typedef struct {
    char pad00[0x24];
    u32 bits0 : 9;                   
    u32 bit9 : 1;
    u32 bit10 : 1;
    u32 bit11 : 1;
    u32 bits12 : 20;
} GameStatusSplitBits;

 
typedef struct {
    char bytes[8];
} PlayTime;

 
typedef struct {
    s32 magic;                       
    s32 size;                        
} SaveHeader;




 
typedef struct {
    f32 position[4];                 
    f32 unk10;                       
    f32 unk14;                       
    f32 unk18;                       
    char pad1C[0x04];
    f32 unk20[4];                    
    f32 unk30[4];                    
    f32 unk40;                       
    f32 unk44;                       
    char pad48[0x04];
    f32 unk4C;                       
    f32 unk50[4];                    
    f32 unk60[4];                    
    f32 unk70;                       
    s32 unk74;                       
    f32 unk78;                       
    s32 unk7C;                       
} CameraState;

 
typedef struct MessageSlot {
    char *text;                      
    int unk04;                       
    int unk08;                       
    int length;                      
    struct MessageSlot *next;        
    int unk14;                       
    int unk18;                       
    int unk1C;                       
    int unk20;                       
} MessageSlot;

 
typedef struct {
    f32 step_s;                      
    f32 step_t;                      
    f32 size_s;                      
    f32 size_t;                      
    f32 base_s;                      
    f32 base_t;                      
    s32 columns;                     
} GlyphSheet;

 
typedef struct {
    s32 left;                        
    s32 top;                         
    s32 right;                       
    s32 bottom;                      
} GlyphBox;

 
typedef struct {
    f32 unk00[4];                    
    f32 unk10[4];                    
    f32 unk20[4];                    
    f32 unk30[4];                    
    f32 unk40;                       
    f32 unk44;                       
    f32 unk48;                       
    f32 unk4C;                       
    void *current;                   
    void *unk54;                     
    void *unk58;                     
} EventRunner;

 
typedef struct {
    char pad00[0x10];
    u32 flags;                       
} GameEvent;

 
typedef struct {
    void (*run)(void);               
    int unk04;                       
    int count;                       
} ViewerEntry;

 
typedef struct {
    s32 values[4];                   
} ViewerCornerTable;

 
typedef struct {
    char unk0;                       
    u8 sec;                          
    u8 min;                          
    u8 hour;                         
    char unk4;                       
    u8 mday;                         
    u8 mon;                          
    u8 year;                         
} ClockTime;

 
typedef struct {
    s16 unk00[9];                    
    u8 level;                        
    char pad13[0x05];
    u8 unk18;                        
    ClockTime unk19;                 
    char pad21[0x01];
} LevelRecordTimed;

 
typedef struct {
    char pad00[0x08];
    int ticks;                       
} GameClock;

 
typedef struct {
    char pad00[0x208];
    char unk208;                     
    char unk209;                     
    char unk20A;                     
    char unk20B;                     
    char pad20C[0x7];
    char unk213;                     
    char unk214;                     
    char pad215[0x73];
    int flags;                       
} ClockControl;

 
typedef struct {
    u8 delay;                        
    u8 interval;                     
    u8 countdown;                    
    u8 fired;                        
} PadRepeatKey;

 
typedef struct {
    u8 dma_buffer[0x100];            
    u8 report_status;                
    u8 report_id;                    
    u16 buttons;                     
    u8 report_analog[0x1C];          
    u16 pressed;                     
    u16 repeated;                    
    PadRepeatKey repeat_keys[16];    
    u16 repeat_mask;                 
    char pad166[0x02];
    int state;                       
    int request_state;               
    int actuator_count;              
    u8 actuators[6];                 
    u8 phase;                        
    u8 next_phase;                   
    u8 pad_type;                     
    u8 features;                     
    u8 mode;                         
    u8 mode_request;                 
    u8 mode_lock;                    
    u8 actuators_aligned;            
    char pad182[0x3E];
} PadSlot;

 
typedef struct {
    PadSlot slots[8];                
    u8 open_slots[2];                
} PadSystem;

 
typedef struct {
    int tex_u;                       
    int tex_v;                       
    char pad08[0x08];
    int red;                         
    int green;                       
    int blue;                        
    int alpha;                       
    int pos_x;                       
    int pos_y;                       
    int pos_z;                       
    char pad2C[0x04];
} TextLogVertex;

 
typedef struct {
    TextLogVertex corners[2];        
} TextLogGlyph;

 
typedef struct {
    int tag[4];                      
    TextLogGlyph glyphs[37];         
} TextLogPacket;

 
typedef struct {
    TextLogGlyph *packet;            
    char pad04[0x0C];
    int left;                        
    int top;                         
    int width;                       
    int height;                      
    int depth;                       
    char pad24[0x0C];
    int red;                         
    int green;                       
    int blue;                        
    char pad3C[0x04];
    int font_base;                   
    int palette_base;                
    int font_buffer_width;           
    int used;                        
    char text[0x800];                
} TextLog;

 
typedef struct {
    char flag0;                      
    char flag1;                      
    char pad02[0x02];
    int words[10];                   
    char pad2C[0x14C];
    PlayTime tail;                   
} ScratchSlot;

 
typedef struct {
    char pad00[0x30];
    u16 unk30;                       
    char pad32[0x01];
    u8 unk33;                        
    char pad34[0x08];
    u8 unk3C;                        
    char pad3D[0x01];
    u16 unk3E;                       
} GameDefinition;

 
typedef struct {
    char pad00[0x30];
    u32 file_number : 16;            
    u32 unk30_16 : 16;
    char pad34[0x0C];
} GameDefinitionWords;

 
typedef struct {
    char pad00[0x30];
    u32 flags;                       
    char pad34[0x0C];
} GameDefinitionFlags;

 
typedef struct {
    char pad00[0x30];
    u32 file_number : 16;            
    u32 unk30_16 : 2;
    u32 state : 2;                   
    u32 unk30_20 : 12;
    char pad34[0x0C];
} GameDefinitionBits;

 
typedef struct {
    u8 bytes[380];
} DefinitionByteSet;

 
typedef struct {
    u8 bits[8];
} AreaFlagBits;

 
typedef struct {
    int value;
} __attribute__((packed)) UnalignedInt;

 
typedef struct {
    int x[3];
} LayerOffsets;

 
typedef struct {
    int points[8][11][2];
} __attribute__((aligned(16))) RippleGrid;

 
typedef struct ListNode {
    char pad00[0x20];
    struct ListNode *prev;           
    struct ListNode *next;           
} ListNode;

 
typedef struct {
    char pad00[0x04];
    int unk04;                       
} PendingSlot;

 
typedef struct {
    s16 *start;                      
    s16 *cursor;                     
    s16 *end;                        
    int delay;                       
    int timer;                       
    int unk14;                       
    int waiting;                     
    int typed;                       
    int closing;                     
    int close_timer;                 
    int left;                        
    int top;                         
    int font;                        
    int unk34;                       
    int line_offsets[4];             
} MessageBox;

 
typedef struct PoolSlot {
    char pad00[0x40];
    struct PoolSlot *unk40;          
    struct PoolPart *part;           
    struct PoolPartRecord *unk48;    
    char pad4C[0x04];
    struct PoolSlot *next;           
    s32 key_index;                   
    u64 unk58;                       
    u64 unk60;                       
    char pad68[0x08];
} PoolSlot;

 
typedef struct {
    u64 tex0;                        
    u64 unk08;                       
} PoolSlotRegisters;

 
typedef struct {
    char pad00[0x58];
    PoolSlotRegisters registers;     
    char pad68[0x08];
} PoolSlotRegistersView;



 
typedef struct {
    char pad00[0x08];
    void *unk08;                     
    void *unk0C;                     
} RelocLayer;

 
typedef struct {
    u16 key_count;                   
    u16 layer_count;                 
    u16 slot;                        
    char pad06[0x2A];
    void *unk30;                     
    RelocLayer *layers;              
    void *unk38;                     
    void *unk3C;                     
} RelocEntry;

 
typedef struct {
    char pad00[0x08];
    u32 entry_count;                 
    char pad0C[0x04];
    RelocEntry entries[1];           
} RelocFile;

 
typedef struct {
    u8 slots[7];                     
} PoolSlotList7;

typedef struct {
    u8 slots[4];                     
} PoolSlotList4;

typedef struct {
    u8 slots[2];                     
} PoolSlotList2;

typedef struct {
    u8 slots[6];                     
} PoolSlotList6;

typedef struct {
    u32 counts[6];                   
} PoolSlotCounts6;

typedef struct {
    u32 counts[10];                  
} PoolSlotCounts10;

typedef struct {
    u32 values[11];                  
} PoolSlotValues11;

 
typedef struct {
    u16 point;                       
    u16 normal;                      
    char pad04[0x04];
    f32 tex_s;                       
    f32 tex_t;                       
    f32 color[4];                    
} PoolPartPoint;

 
typedef struct {
    u16 point_count;                 
    char pad02[0x02];
    u16 unk04;                       
    u16 prim;                        
    u64 tex0;                        
    u64 unk10;                       
    char pad18[0x08];
    PoolPartPoint points[1];         
} PoolPartMaterial;

 
typedef struct {
    char pad00[0x08];
    u64 pad08 : 61;
    u64 cld : 3;                     
} PoolPartMaterialBits;

 
typedef struct {
    u32 prim_tag[4];                 
    u32 reg_tag[4];                  
    s32 packet_size;                 
    char pad24[0x4C];
    s32 point;                       
    u32 strip;                       
    u32 point_count;                 
    u32 normal_count;                
    u32 strip_count;                 
    char pad84[0x1C];
    f32 unkA0[4];                    
    f32 screen[16];                  
    f32 light[16];                   
    f32 world[16];                   
    u64 tex0;                        
    PoolPartMaterial *strip_start;   
    u64 *registers;                  
    u32 *packet;                     
    PoolPartPoint *strip_point;      
    PoolPartMaterial *strip_at;      
    u32 *cursor;                     
    f32 (*screen_points)[4];         
    f32 (*work)[4];                  
    f32 (*normals)[4];               
    char pad19C[0x04];
    s32 unk1A0[4];                   
    u32 parent_point_count;          
    u32 point_at;                    
    u32 point_index;                 
    u32 parent_normal_count;         
    u32 normal;                      
    u32 normal_index;                
} PoolDrawScratch;



 
typedef struct {
    char pad000[0x18C];
    union {
        u32 *words;                  
        u8 *bytes;
    } cursor;
} PoolDrawScratchCursorView;



 
typedef struct {
    u64 pages[2];                    
} PoolClutPages;

 
typedef struct {
    u16 frame;                       
    u16 length;                      
    char pad04[0x0C];
    f32 translation[4];              
    f32 translation_in[4];           
    f32 translation_out[4];          
    f32 rotation[4];                 
    f32 rotation_in[4];              
    f32 rotation_out[4];             
    f32 scale[4];                    
} PoolNodeKey;

 
typedef struct PoolPart {
    f32 position[4];                 
    f32 rotation[4];                 
    f32 scale[4];                    
    u16 strip_count;                 
    char pad32[0x02];
    PoolPartMaterial *material;      
    u16 point_count;                 
    char pad3A[0x02];
    f32 (*points)[4];                
    u16 normal_count;                
    char pad42[0x02];
    f32 (*normals)[4];               
    char pad48[0x08];
} PoolPart;

 
typedef struct PoolPartRecord {
    s32 unk00;                       
    s32 unk04;                       
    s32 unk08;                       
    char pad0C[0x04];
} PoolPartRecord;

 
typedef struct {
    PoolPartRecord *records_end;     
    char pad04[0x04];
    u32 part_count;                  
    char pad0C[0x04];
    PoolPart parts[1];               
} PoolPartFile;

 
typedef struct {
    PoolPartRecord *records_end;     
    char pad04[0x04];
    u32 part_count;                  
    char pad0C[0x04];
} PoolPartFileHeader;

 
typedef union {
    u64 words[4];                    
    char bytes[0x20];                
} PoolPartStripWords;

 
typedef struct {
    f32 position[4];                 
    f32 normal[4];                   
    f32 tex_s;                       
    f32 tex_t;                       
    f32 tex_q;                       
    f32 unk2C;                       
    f32 color[4];                    
} PoolPartFlatPoint;

 
typedef struct {
    u16 point_count;                 
    char pad02[0x1E];
    PoolPartFlatPoint points[1];     
} PoolPartFlatStrip;

 
typedef struct {
    PoolPartFile *file;              
    void *texture;                   
} PoolPartSource;

 
typedef struct {
    u16 held;                        
    u16 pressed;                     
    s32 type;                        
    u16 look_buttons;                
    u16 prev_look_buttons;           
    u16 move_buttons;                
    u16 prev_move_buttons;           
} PadState;

 
typedef struct {
    char pad00[0x02];
    u16 held;                        
    u8 right_stick_x;                
    u8 right_stick_y;                
    u8 left_stick_x;                 
    u8 left_stick_y;                 
    char pad08[0x18];
    u16 pressed;                     
} PadReport;

 
typedef struct {
    char pad00[0x04];
    u16 right_stick;                 
    u16 left_stick;                  
} PadReportAxes;

 
typedef union {
    u16 buttons;
    u8 bytes[2];
} PadButtonsUnion;

typedef struct {
    char pad00[0x20];
    PadButtonsUnion pressed;         
} PadReportUnion;

 
typedef struct {
    char text[0x1A];
} DebugStartMenuSpacer;

typedef struct {
    char text[0x46];
} DebugStartMenuPrompt;

typedef struct {
    char text[4][0x80];
} DebugStartMenuItems;

 
typedef struct {
    char pad00[0x70];
    s16 *unk70;                      
} Unk20D008Owner;



 






 
typedef struct {
    f32 point[4];                    
    s32 hit;                         
    f32 distance;                    
    s32 unk18;                       
    char pad1C[0x04];
    f32 normal[4];                   
} CollisionResult;



 


 
typedef struct {
    CollisionResult result;          
    char pad30[0x40];
    Vector4 push;                    
    Vector4 point;                   
    Vector4 unk90;                   
} CollisionScratch;



 
typedef union {
    CollisionResult result;
    s64 doublewords[6];
} CollisionResultCopy;



 
typedef struct {
    f32 point[4];                    
    s32 hit;                         
    f32 distance;                    
    s32 unk18;                       
    char pad1C[0x04];
    f32 normal[4];                   
} __attribute__((aligned(8))) CollisionResultAligned;



 
typedef struct {
    void *position;                  
    s32 unk04;                       
    s32 unk08;                       
    s32 unk0C;                       
} CollisionStripPoint;

 
typedef struct {
    u16 point_count;                 
    char pad02[0x1E];
    CollisionStripPoint points[1];   
} CollisionStrip;

 
typedef struct {
    s32 strip_count;                 
    CollisionStrip *strips;          
    char *unk08;                     
    u32 unk0C;                       
    f32 unk10;                       
    char *unk14;                     
    CollisionStrip *unk18;           
    CollisionStrip *floor_strips;    
    CollisionStrip *unk20;           
    CollisionStrip *unk24;           
    CollisionStrip *unk28;           
    CollisionStrip *unk2C;           
    char pad30[0x08];
    u16 unk38;                       
    u16 floor_strip_count;           
    u16 unk3C;                       
    u16 unk3E;                       
    u16 unk40;                       
    u16 unk42;                       
} CollisionMesh;

 
typedef struct {
    char pad00[0x18];
    CollisionStrip *strip_lists[7];  
    char pad34[0x04];
    u16 strip_counts[7];             
} CollisionMeshLists;

 
typedef struct {
    f32 position[4];                 
    f32 rotation[4];                 
    f32 scale[4];                    
    u16 unk30;                       
    u16 unk32;                       
    char pad34[0x04];
    u8 unk38;                        
    char pad39[0x03];
    u32 unk3C;                       
    f32 unk40[4];                    
    f32 unk50[4];                    
    f32 lowest[4];                   
    f32 highest[3];                  
    ModelInstance *model;            
} MapRecordBounds;

 
typedef struct {
    s32 lists[4];                    
} CollisionListPick4;

typedef struct {
    s32 lists[2];                    
} CollisionListPick2;




typedef struct {
    f32 unk00[4][4];
    f32 unk40[4][4];
} __attribute__((aligned(8))) MapLightBlock;

typedef struct {
    MapLightBlock blocks[0x20];
} MapLightRow;

typedef struct {
    char pad00[0x50];
    u8 fog_colors[4][4];
    char pad60[0x1C];
    f32 draw_distance;
} MapHeaderFog;

typedef struct {
    char pad00[0x38];
    u64 pad38_0 : 26;
    u64 in_view : 1;
    u64 map : 1;
    u64 pad38_28 : 36;
} MapRecordBits2;


typedef struct {
    char pad00[0x60];
    f32 mirror_height;
    u8 mode;
    char pad65[0x35];
    u8 unk9A;
    u8 unk9B;
    u8 unk9C;
    char pad9D[0x03];
    MapSlotUnk0CValues unkA0;
    MapLightBlock unkC0;
    MapSlotUnk0CValues unk140;
    MapSlotUnk0CValues unk160;
    MapLightBlock unk180;
    MapLightBlock unk200;
    u8 fog[4];
    u8 fog_from[4];
    u8 fog_to[4];
    char pad28C[0x04];
    MapSlotUnk0CValues unk290[2][0x20];
    MapLightRow unkA90[2];
    f32 unk2A90;
    f32 unk2A94;
    f32 unk2A98;
    char pad2A9C[0x04];
} MapDrawStateBlend;

extern MapSlot D_0038CD20[];
extern MapDrawStateBlend D_0038A1B0;
extern PlayerState D_00413F80;
extern CameraState D_004325A0;
extern f32 D_00431AF0[16];
extern f32 D_004323E0[16];
extern MapDrawRecord *D_00446778[2];
extern MapDrawRecord *D_00446780[2];
extern int D_00446770;

void *memset(void *, int, u32);
float cosf(float);
float sinf(float);
void func_00116F90(void);
void func_001175F0(void);
void Memory_CopyWords(s32 *, s32 *, u32);
int func_00116C08(int, int, int, int);
void func_00112018(ModelInstance *);
void func_00216778(void *, void *);
void func_00216B48(void *, void *, void *);
void func_0020B8B0(ModelInstance *, ModelPart *, ModelPart *);
void sceVu0CopyMatrix(void *, void *);
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0RotMatrixZ(float *, float *, float);
void sceVu0TransMatrix(float *, float *, float *);
















typedef struct {
    u8 red;
    u8 green;
    u8 blue;
} MapFogRgb;


void func_00112BD0(void) {
    PERM_GENERAL(PERM_VAR(fade_outer, f32 fade_out; f32 fade_in;) PERM_VAR(fade_inner, ), PERM_VAR(fade_outer, ) PERM_VAR(fade_inner, f32 fade_out; f32 fade_in;))
    PERM_GENERAL(PERM_VAR(S, D_0038A1B0), PERM_VAR(S, (*state)))
    PERM_GENERAL(PERM_VAR(F, ((MapFogRgb *)&PERM_VAR(S).unk140.unk10)) PERM_VAR(T, ((MapFogRgb *)&PERM_VAR(S).unk160.unk10)) PERM_VAR(D, ((MapFogRgb *)&PERM_VAR(S).unkA0.unk10)), PERM_VAR(F, from_rgb) PERM_VAR(T, to_rgb) PERM_VAR(D, out_rgb))
    f32 unused_vector[4];
    f32 unused_matrix[32];
    MapSlot *slot;
    MapDrawRecord *record;
    MapLightBlock *lights;
    MapSlotUnk0CValues *fog_block;
    u8 *fog;
    f32 cos_heading;
    f32 sin_heading;
    f32 player_x;
    f32 player_z;
    PERM_VAR(fade_outer)
    f32 color_out;
    f32 color_in;
    f32 light_out;
    f32 light_in;
    f32 delta_x;
    f32 delta_z;
    f32 distance;
    f32 side_x0;
    f32 side_x1;
    f32 side_z0;
    f32 side_z1;
    int i;
    int j;
    int k;
    int value;
    PERM_GENERAL(int, unsigned int, u8, s16) blended;

    slot = &D_0038CD20[D_00446770];
    record = (MapDrawRecord *)slot->records + D_00413F80.map_index;
    fog_block = &D_0038A1B0.unk290[D_00446770][record->unk34];
    if ((D_0038A1B0.unk9A == 0 && ((D_0038A1B0.unkA0).unk08 != ( fog_block)->unk08 || (D_0038A1B0.unkA0).unk0C != ( fog_block)->unk0C || (D_0038A1B0.unkA0).unk10 != ( fog_block)->unk10)) || (D_0038A1B0.unk9A != 0 && ((D_0038A1B0.unk160).unk08 != ( fog_block)->unk08 || (D_0038A1B0.unk160).unk0C != ( fog_block)->unk0C || (D_0038A1B0.unk160).unk10 != ( fog_block)->unk10))) {
        D_0038A1B0.unk9A = 30;
        D_0038A1B0.unk140 = D_0038A1B0.unkA0;
        D_0038A1B0.unk160 = *fog_block;
    }
    if (D_0038A1B0.unk9A != 0) {
        D_0038A1B0.unk9A--;
        if (D_0038A1B0.unk9A != 0) {
            {
                MapDrawStateBlend *state = &D_0038A1B0;
                MapFogRgb *from_rgb = (MapFogRgb *)&PERM_VAR(S).unk140.unk10;
                MapFogRgb *to_rgb = (MapFogRgb *)&PERM_VAR(S).unk160.unk10;
                MapFogRgb *out_rgb = (MapFogRgb *)&PERM_VAR(S).unkA0.unk10;
                f32 blend_sum;
                f32 green_from;
                f32 green_to;
                f32 blue_from;
                f32 blue_to;
                f32 far_to;
                f32 red_from;
                f32 red_to;
                PERM_VAR(fade_inner)
                fade_out = PERM_VAR(S).unk9A;
                fade_in = 30 - PERM_VAR(S).unk9A;
                fade_out /= 30.0f;
                fade_in /= 30.0f;
                PERM_GENERAL(blend_sum = PERM_VAR(F)->red * fade_out + PERM_VAR(T)->red * fade_in; blended = (int)(blend_sum); PERM_VAR(D)->red = blended;, red_from = PERM_VAR(F)->red; red_to = PERM_VAR(T)->red; red_from *= fade_out; red_to *= fade_in; red_from += red_to; blended = (int)(red_from); PERM_VAR(D)->red = blended;)
                PERM_GENERAL(green_from = PERM_VAR(F)->green; green_to = PERM_VAR(T)->green; green_from *= fade_out; green_to *= fade_in; green_from += green_to; blended = (int)(green_from); PERM_VAR(D)->green = blended;, blend_sum = PERM_VAR(F)->green * fade_out + PERM_VAR(T)->green * fade_in; blended = (int)(blend_sum); PERM_VAR(D)->green = blended;)
                PERM_GENERAL(blue_from = PERM_VAR(F)->blue; blue_to = PERM_VAR(T)->blue; blue_from *= fade_out; blue_to *= fade_in; blue_from += blue_to; blended = (int)(blue_from); PERM_VAR(D)->blue = blended;, blend_sum = PERM_VAR(F)->blue * fade_out + PERM_VAR(T)->blue * fade_in; blended = (int)(blend_sum); PERM_VAR(D)->blue = blended;)
                PERM_GENERAL(far_to = PERM_VAR(S).unk160.unk0C; far_to *= fade_in; PERM_VAR(S).unkA0.unk0C = PERM_VAR(S).unk140.unk0C * fade_out + far_to;, PERM_VAR(S).unkA0.unk0C = PERM_VAR(S).unk140.unk0C * fade_out + PERM_VAR(S).unk160.unk0C * fade_in;)
                PERM_GENERAL(blend_sum = PERM_VAR(S).unk140.unk08 * fade_out + PERM_VAR(S).unk160.unk08 * fade_in; PERM_VAR(S).unkA0.unk08 = blend_sum;, PERM_VAR(S).unkA0.unk08 = PERM_VAR(S).unk140.unk08 * fade_out + PERM_VAR(S).unk160.unk08 * fade_in;)
            }
        } else {
            D_0038A1B0.unkA0 = D_0038A1B0.unk160;
        }
    }
}
