#ifndef BLOCK_H
#define BLOCK_H
#include <renderer.h>
#include <stdint.h>
#include <d3d11_1.h>



typedef struct __BLOCK_MODEL_ARRAY BlockModelArray;
typedef struct __BLOCK_MODEL* BlockModel;
typedef struct __BLOCK_MODEL_CONNECTION* BlockModelConnection;
typedef uint64_t BlockModelId;



struct __BLOCK_MODEL_ARRAY{
	size_t ln;
	struct __BLOCK_MODEL* e;
};



struct __BLOCK_MODEL_CONNECTION{
	uint8_t nml;
	char* nm;
	float sz;
	float x;
	float y;
	float z;
	float rx;
	float ry;
	float rz;
	float dx;
	float dy;
	float dz;
};



struct __BLOCK_MODEL{
	BlockModelId id;
	uint8_t nml;
	char* nm;
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t fl;
	BlockModelId* ch;
	uint8_t cnl;
	struct __BLOCK_MODEL_CONNECTION* cn;
	uint32_t ill;
	ID3D11Buffer* ib;
	ID3D11Buffer* vb;
	uint64_t _fsz;
	char* _fp;
};



extern BlockModelArray blk_l;



void init_block_list(void);



BlockModel get_block_model_by_id(BlockModelId id);



void draw_block_model(BlockModel b,Matrix tm,Matrix rm,Vector cl);



#endif
