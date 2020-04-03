/* **************************************************************************** */
/*																				*/
/*   M3D Shaders Management													*/
/*	 Nicolas Cannasse															*/
/*	 (c)2002 Motion-Twin														*/
/*																				*/
/* **************************************************************************** */
#include <m3d.h>
#include <wpairbtree.h>

namespace M3D {

int cmp_keys( WString *a, WString *b ) { return strcmp(a->c_str(),b->c_str()); }
void release_shader( WString *name, DWORD shader, void * ) {
	delete name;
	device->DeleteVertexShader(shader);
}

WPairBTree<WString*,DWORD> shaders(cmp_keys);
extern bool use_hwshaders;

/* ---------------------------------------------------------------------------- */

bool
Shader::Add( WString name, const char *code_asm, DWORD *decl ) {
	LPD3DXBUFFER error = NULL;
	LPD3DXBUFFER shader_code = NULL;
	if( !M3DCHECK(D3DXAssembleShader(code_asm,strlen(code_asm),0,NULL,&shader_code,&error)) ) {
		M3DTRACE((char*)error->GetBufferPointer());
		error->Release();
		return false;
	}

	DWORD shader;
	if( !M3DCHECK(device->CreateVertexShader(decl,(DWORD*)shader_code->GetBufferPointer(),&shader,use_hwshaders?0:D3DUSAGE_SOFTWAREPROCESSING)) ) {
		shader_code->Release();
		M3DTRACE("Device.CreateVertexShader failed");
		return false;
	}
	shader_code->Release();

	shaders.Add(new WString(name),shader);
	return true;
}

/* ---------------------------------------------------------------------------- */

bool
Shader::Set( WString name ) {
	DWORD shader = shaders.Find(&name);
	if( !shader ) {
		M3DTRACE("Unknown shader "+name);
		return false;
	}
	device->SetVertexShader(shader);
	return true;
}

/* ---------------------------------------------------------------------------- */

void
Shader::Clean() {
	shaders.Iter(release_shader,NULL);
	shaders.Clean();
}

/* ---------------------------------------------------------------------------- */
}; // namespace
/* **************************************************************************** */