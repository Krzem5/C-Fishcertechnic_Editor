#include <editor.h>
#include <renderer.h>
#include <block.h>
#include <ui.h>
#include <color_3d_pixel.h>
#include <color_3d_vertex.h>
#include <stdio.h>



#define EDITOR_BG_COLOR_R 0.91f
#define EDITOR_BG_COLOR_G 0.91f
#define EDITOR_BG_COLOR_B 0.91f



typedef struct _CAMERA* Camera;



struct _CAMERA{
	float x;
	float y;
	float z;
	float rx;
	float ry;
	float rz;
	float zm;
	float ms;
	float rs;
	bool enabled;
	uint16_t _f;
	int16_t _rx;
	int16_t _ry;
	int16_t _px;
	int16_t _py;
	float _ox;
	float _oy;
	float _oz;
};




Camera create_camera(float ms,float rs,float x,float y,float z,float rx,float ry,float rz,float zm){
	Camera o=malloc(sizeof(struct _CAMERA));
	o->x=x;
	o->y=y;
	o->z=z;
	o->rx=(rx-90)*PI_DIV_180;
	o->ry=ry*PI_DIV_180;
	o->rz=rz*PI_DIV_180;
	o->zm=zm;
	o->ms=ms;
	o->rs=rs;
	o->enabled=false;
	o->_f=0;
	o->_rx=0;
	o->_ry=0;
	o->_px=0;
	o->_py=0;
	o->_ox=0;
	o->_oy=0;
	o->_oz=0;
	return o;
}



Matrix update_camera(Camera c,float dt){
	if (c->enabled==true){
		bool rc=false;
		if ((renderer_mf&M_RIGHT)!=0){
			rc=true;
			if ((c->_f&M_RIGHT)==0){
				c->_f|=M_RIGHT;
				c->_rx=renderer_mx;
				c->_ry=renderer_my;
			}
			c->rx-=(c->_ry-renderer_my)*c->rs*dt*PI_DIV_180;
			c->ry-=(c->_rx-renderer_mx)*c->rs*dt*PI_DIV_180;
			if (c->rx>PI_DIV_TWO-EPSILON){
				c->rx=PI_DIV_TWO-EPSILON;
			}
			else if (c->rx<-PI_DIV_TWO+EPSILON){
				c->rx=-PI_DIV_TWO+EPSILON;
			}
			c->_rx=renderer_mx;
			c->_ry=renderer_my;
		}
		else{
			c->_f&=(~M_RIGHT);
		}
		float dx=cosf(c->ry)*c->ms*dt;
		float dz=sinf(c->ry)*c->ms*dt;
		if (is_pressed(0x57)){
			rc=true;
			c->_ox+=dx;
			c->_oz+=dz;
		}
		if (is_pressed(0x53)){
			rc=true;
			c->_ox-=dx;
			c->_oz-=dz;
		}
		if (is_pressed(0x41)){
			rc=true;
			c->_ox+=dz;
			c->_oz-=dx;
		}
		if (is_pressed(0x44)){
			rc=true;
			c->_ox-=dz;
			c->_oz+=dx;
		}
		if (is_pressed(VK_SPACE)){
			rc=true;
			c->_oy+=c->ms*dt;
		}
		if (is_pressed(VK_LSHIFT)){
			rc=true;
			c->_oy-=c->ms*dt;
		}
		if (rc==true){
			c->x=c->_ox+c->zm*sinf(-c->rx+PI_DIV_TWO)*cosf(PI-c->ry);
			c->y=c->_oy+c->zm*cosf(-c->rx+PI_DIV_TWO);
			c->z=c->_oz+c->zm*sinf(-c->rx+PI_DIV_TWO)*sinf(PI-c->ry);
		}
	}
	return look_at_matrix(c->x,c->y,c->z,sinf(c->rx-PI_DIV_TWO)*cosf(c->ry),cosf(c->rx-PI_DIV_TWO),sinf(c->rx-PI_DIV_TWO)*sinf(c->ry),0,1,0);
}



struct CBufferLayout{
	RawVector cd;
	RawMatrix cm;
	RawMatrix pm;
};



int64_t g_sz[]={-10,-10,10,10};
ID3D11Buffer* g_ib=NULL;
size_t g_ib_l=0;
ID3D11Buffer* g_vb=NULL;
ID3D11VertexShader* vs=NULL;
ID3D11PixelShader* ps=NULL;
ID3D11Buffer* cb=NULL;
RawMatrix pm;
Camera c;
BlockModel mdl;



void run_editor(void){
	init_renderer();
}



void init_editor(void){
	D3D11_INPUT_ELEMENT_DESC il[]={
		{
			"SV_POSITION",
			0,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			0,
			0,
			D3D11_INPUT_PER_VERTEX_DATA
		},
		{
			"COLOR",
			0,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			0,
			D3D11_APPEND_ALIGNED_ELEMENT,
			D3D11_INPUT_PER_VERTEX_DATA
		}
	};
	vs=load_vertex_shader(g_color_3d_vs,sizeof(g_color_3d_vs),il,sizeof(il)/sizeof(D3D11_INPUT_ELEMENT_DESC));
	ps=load_pixel_shader(g_color_3d_ps,sizeof(g_color_3d_ps));
	cb=create_constant_buffer(sizeof(struct CBufferLayout));
	*renderer_cc=EDITOR_BG_COLOR_R;
	*(renderer_cc+1)=EDITOR_BG_COLOR_G;
	*(renderer_cc+2)=EDITOR_BG_COLOR_B;
	*(renderer_cc+3)=1;
	c=create_camera(10,500,0,0,0,90,0,0,1);
	c->enabled=true;
	float n=EPSILON;
	float f=1000;
	pm=raw_matrix(1/renderer_aspect_ratio,0,0,0,0,1,0,0,0,0,-2/(f-n),-(f+n)/(f-n),0,0,0,1);
	init_block_list();
	mdl=get_block_model_by_id(78238);
	/*22429,1835010,1835011,1048696,36734,36770*/
	init_ui();
}



void update_editor(double dt){
	static float t=0;
	t+=(float)(dt*1e-6);
	update_ui(dt);
	Matrix cm=update_camera(c,(float)(dt*1e-6));
	if (cm==NULL){
		return;
	}
	if (renderer_w==NULL){
		return;
	}
	static bool rd=true;
	if (rd==true){
		g_ib_l=(*(g_sz+2)-(*g_sz)+(*(g_sz+3))-(*(g_sz+1))+2)*2;
		size_t vlsz=(*(g_sz+2)-(*g_sz)+(*(g_sz+3))-(*(g_sz+1))+2)*2*8;
		uint16_t* il=malloc(g_ib_l*sizeof(uint16_t));
		float* vl=malloc(vlsz*sizeof(float));
		size_t j=0;
		uint16_t k=0;
		for (int64_t i=*g_sz;i<=*(g_sz+2);i++){
			*(vl+j)=(float)i;
			*(vl+j+1)=0;
			*(vl+j+2)=(float)(*(g_sz+1));
			*(vl+j+3)=1;
			*(vl+j+4)=0.1f;
			*(vl+j+5)=0.1f;
			*(vl+j+6)=0.1f;
			*(vl+j+7)=1;
			*(vl+j+8)=(float)i;
			*(vl+j+9)=0;
			*(vl+j+10)=(float)*(g_sz+3);
			*(vl+j+11)=1;
			*(vl+j+12)=0.1f;
			*(vl+j+13)=0.1f;
			*(vl+j+14)=0.1f;
			*(vl+j+15)=1;
			*(il+k)=k;
			*(il+k+1)=k+1;
			j+=16;
			k+=2;
		}
		for (int64_t i=*(g_sz+1);i<=*(g_sz+3);i++){
			*(vl+j)=(float)(*g_sz);
			*(vl+j+1)=0;
			*(vl+j+2)=(float)i;
			*(vl+j+3)=1;
			*(vl+j+4)=0.1f;
			*(vl+j+5)=0.1f;
			*(vl+j+6)=0.1f;
			*(vl+j+7)=1;
			*(vl+j+8)=(float)(*(g_sz+2));
			*(vl+j+9)=0;
			*(vl+j+10)=(float)i;
			*(vl+j+11)=1;
			*(vl+j+12)=0.1f;
			*(vl+j+13)=0.1f;
			*(vl+j+14)=0.1f;
			*(vl+j+15)=1;
			*(il+k)=k;
			*(il+k+1)=k+1;
			j+=16;
			k+=2;
		}
		if (g_vb==NULL||g_ib==NULL){
			assert(g_vb==NULL);
			assert(g_ib==NULL);
			D3D11_BUFFER_DESC bd={
				(uint32_t)(g_ib_l*sizeof(uint16_t)),
				D3D11_USAGE_DYNAMIC,
				D3D11_BIND_INDEX_BUFFER,
				D3D11_CPU_ACCESS_WRITE,
				0,
				0
			};
			D3D11_SUBRESOURCE_DATA dt={
				il,
				0,
				0
			};
			HRESULT hr=ID3D11Device_CreateBuffer(renderer_d3_d,&bd,&dt,&g_ib);
			bd.ByteWidth=(uint32_t)(vlsz*sizeof(float));
			bd.BindFlags=D3D11_BIND_VERTEX_BUFFER;
			dt.pSysMem=vl;
			hr=ID3D11Device_CreateBuffer(renderer_d3_d,&bd,&dt,&g_vb);
		}
		else{
			ID3D11Resource* vbr;
			ID3D11Buffer_QueryInterface(g_vb,&IID_ID3D11Resource,(void**)&vbr);
			D3D11_MAPPED_SUBRESOURCE mr;
			HRESULT hr=ID3D11DeviceContext_Map(renderer_d3_dc,vbr,0,D3D11_MAP_WRITE_DISCARD,0,&mr);
			float* nvl=(float*)(mr.pData);
			for (size_t i=0;i<vlsz;i++){
				*(nvl+i)=*(vl+i);
			}
			ID3D11DeviceContext_Unmap(renderer_d3_dc,vbr,0);
			ID3D11Resource_Release(vbr);
			ID3D11Resource* ibr;
			ID3D11Buffer_QueryInterface(g_ib,&IID_ID3D11Resource,(void**)&ibr);
			hr=ID3D11DeviceContext_Map(renderer_d3_dc,ibr,0,D3D11_MAP_WRITE_DISCARD,0,&mr);
			uint16_t* nil=(uint16_t*)(mr.pData);
			for (size_t i=0;i<g_ib_l;i++){
				*(nil+i)=*(il+i);
			}
			ID3D11DeviceContext_Unmap(renderer_d3_dc,ibr,0);
			ID3D11Resource_Release(ibr);
		}
		free(il);
		free(vl);
	}
	ID3D11DeviceContext_VSSetShader(renderer_d3_dc,vs,NULL,0);
	ID3D11DeviceContext_PSSetShader(renderer_d3_dc,ps,NULL,0);
	struct CBufferLayout cb_dt={
		raw_vector(sinf(c->rx-PI_DIV_TWO)*cosf(c->ry),cosf(c->rx-PI_DIV_TWO),sinf(c->rx-PI_DIV_TWO)*sinf(c->ry),1),
		as_raw_matrix(cm),
		pm
	};
	update_constant_buffer(cb,(void*)&cb_dt);
	ID3D11DeviceContext_VSSetConstantBuffers(renderer_d3_dc,0,1,&cb);
	ID3D11DeviceContext_PSSetConstantBuffers(renderer_d3_dc,0,1,&cb);
	unsigned int off=0;
	unsigned int st=8*sizeof(float);
	ID3D11DeviceContext_IASetVertexBuffers(renderer_d3_dc,0,1,&g_vb,&st,&off);
	ID3D11DeviceContext_IASetIndexBuffer(renderer_d3_dc,g_ib,DXGI_FORMAT_R16_UINT,0);
	ID3D11DeviceContext_IASetPrimitiveTopology(renderer_d3_dc,D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	ID3D11DeviceContext_DrawIndexed(renderer_d3_dc,(uint32_t)g_ib_l,0,0);
	RawMatrix tm=raw_identity_matrix();
	RawMatrix rtm=raw_identity_matrix();
	draw_block_model(mdl,&tm,&rtm,NULL);
	draw_ui();
	free(cm);
}
