#include <editor.h>
#include <renderer.h>
#include <ui.h>
#include <stdio.h>




#define EDITOR_BG_COLOR_R 0.91f
#define EDITOR_BG_COLOR_G 0.91f
#define EDITOR_BG_COLOR_B 0.91f



int64_t g_sz[]={-10,-10,10,10};
ID3D11Buffer* g_ib=NULL;
ID3D11Buffer* g_vb=NULL;



void run_editor(void){
	init_renderer();
}



void init_editor(void){
	init_ui();
	*renderer_cc=EDITOR_BG_COLOR_R;
	*(renderer_cc+1)=EDITOR_BG_COLOR_G;
	*(renderer_cc+2)=EDITOR_BG_COLOR_B;
	*(renderer_cc+3)=1;
}



void update_editor(double dt){
	static float t=0;
	t+=(float)(dt*1e-6);
	update_ui(dt);
	bool rd=true;
	if (rd==true){
		size_t ilsz=(*(g_sz+2)-(*g_sz)+(*(g_sz+3))-(*(g_sz+1))+2)*2;/*ab*/
		size_t vlsz=(*(g_sz+2)-(*g_sz)+(*(g_sz+3))-(*(g_sz+1))-2)*2*8;/*xyzwrgba*/
		uint16_t* il=malloc(ilsz*sizeof(uint16_t));
		float* vl=malloc(vlsz*sizeof(float));
		size_t j=0;
		uint16_t k=0;
		for (int64_t i=(*g_sz);i<=*(g_sz+2);i++){
			*(vl+j)=(float)i;
			*(vl+j+1)=0;
			*(vl+j+2)=-(float)(*(g_sz+1));
			*(vl+j+3)=1;
			*(vl+j+4)=0.1f;
			*(vl+j+5)=0.1f;
			*(vl+j+6)=0.1f;
			*(vl+j+7)=0.1f;
			*(vl+j+8)=(float)i;
			*(vl+j+9)=0;
			*(vl+j+10)=(float)*(g_sz+3);
			*(vl+j+11)=1;
			*(vl+j+12)=0.1f;
			*(vl+j+13)=0.1f;
			*(vl+j+14)=0.1f;
			*(vl+j+15)=0.1f;
			*(il+k)=k;
			*(il+k+1)=k+1;
			j+=16;
			k+=2;
		}
		if (g_vb==NULL||g_ib==NULL){
			assert(g_vb==NULL);
			assert(g_ib==NULL);
			D3D11_BUFFER_DESC bd={
				(uint32_t)(ilsz*sizeof(uint16_t)),
				D3D11_USAGE_DEFAULT,
				D3D11_BIND_INDEX_BUFFER,
				0,
				0,
				0
			};
			D3D11_SUBRESOURCE_DATA dt={
				il,
				0,
				0
			};
			HRESULT hr=ID3D11Device_CreateBuffer(renderer_d3_d,&bd,&dt,&g_ib);
			bd.Usage=D3D11_USAGE_DYNAMIC;
			bd.ByteWidth=(uint32_t)(vlsz*sizeof(float));
			bd.BindFlags=D3D11_BIND_VERTEX_BUFFER;
			bd.CPUAccessFlags=D3D11_CPU_ACCESS_WRITE;
			dt.pSysMem=vl;
			hr=ID3D11Device_CreateBuffer(renderer_d3_d,&bd,&dt,&g_vb);
		}
		else{
			ID3D11Resource* vbr;
			ID3D11Buffer_QueryInterface(g_vb,&IID_ID3D11Resource,(void**)&vbr);
			D3D11_MAPPED_SUBRESOURCE mr;
			HRESULT hr=ID3D11DeviceContext_Map(renderer_d3_dc,vbr,0,D3D11_MAP_WRITE_DISCARD,0,&mr);
			float* nvl=(float*)(mr.pData);
			for (size_t i=0;i<128;i++){
				*(nvl+i)=*(vl+i);
			}
			ID3D11DeviceContext_Unmap(renderer_d3_dc,vbr,0);
			ID3D11Resource_Release(vbr);
			ID3D11Resource* ibr;
			ID3D11Buffer_QueryInterface(g_vb,&IID_ID3D11Resource,(void**)&ibr);
			hr=ID3D11DeviceContext_Map(renderer_d3_dc,ibr,0,D3D11_MAP_WRITE_DISCARD,0,&mr);
			float* nil=(float*)(mr.pData);
			for (size_t i=0;i<128;i++){
				*(nil+i)=*(il+i);
			}
			ID3D11DeviceContext_Unmap(renderer_d3_dc,ibr,0);
			ID3D11Resource_Release(ibr);
		}
		free(il);
		free(vl);
	}
	if (renderer_w==NULL){
		return;
	}
	printf("%f (%f)\n",dt*1e-6,t);
}
