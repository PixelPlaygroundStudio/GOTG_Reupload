#include <wipch.h>

#include "AI_DebugDrawer.h"

#include <glew.h>

class GLCheckerTexture
{
	unsigned int m_texId;
public:
	GLCheckerTexture() : m_texId(0)
	{
	}

	~GLCheckerTexture()
	{
		if (m_texId != 0) {
			GL(DeleteTextures(1, &m_texId));
		}
	}
	void bind()
	{
		if (m_texId == 0)
		{
			// Create checker pattern.
			const unsigned int col0 = duRGBA(215, 215, 215, 255);
			const unsigned int col1 = duRGBA(255, 255, 255, 255);
			static const int TSIZE = 64;
			unsigned int data[TSIZE * TSIZE];

			GL(GenTextures(1, &m_texId));
			GL(BindTexture(GL_TEXTURE_2D, m_texId));

			int level = 0;
			int size = TSIZE;
			while (size > 0)
			{
				for (int y = 0; y < size; ++y)
					for (int x = 0; x < size; ++x)
						data[x + y * size] = (x == 0 || y == 0) ? col0 : col1;
				GL(TexImage2D(GL_TEXTURE_2D, level, GL_RGBA, size, size, 0, GL_RGBA, GL_UNSIGNED_BYTE, data));
				size /= 2;
				level++;
			}

			GL(TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST));
			GL(TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		}
		else
		{
			GL(BindTexture(GL_TEXTURE_2D, m_texId));
		}
	}
};
static GLCheckerTexture g_tex;

void DebugDrawGL::depthMask(bool state)
{
	GL(DepthMask(state ? GL_TRUE : GL_FALSE));
}

void DebugDrawGL::texture(bool state)
{
	if (state)
	{
		GL(Enable(GL_TEXTURE_2D));
		g_tex.bind();
	}
	else
	{
		GL(Disable(GL_TEXTURE_2D));
	}
}

void DebugDrawGL::begin(duDebugDrawPrimitives prim, float size)
{
	switch (prim)
	{
	case DU_DRAW_POINTS:
		GL(PointSize(size));
		GL(Begin(GL_POINTS));
		break;
	case DU_DRAW_LINES:
		GL(LineWidth(size));
		GL(Begin(GL_LINES));
		break;
	case DU_DRAW_TRIS:
		GL(Begin(GL_TRIANGLES));
		break;
	case DU_DRAW_QUADS:
		GL(Begin(GL_QUADS));
		break;
	};
}

void DebugDrawGL::vertex(const float* pos, unsigned int color)
{
	GL(Color4ubv((GLubyte*)&color));
	GL(Vertex3fv(pos));
}

void DebugDrawGL::vertex(const float x, const float y, const float z, unsigned int color)
{
	GL(Color4ubv((GLubyte*)&color));
	GL(Vertex3f(x, y, z));
}

void DebugDrawGL::vertex(const float* pos, unsigned int color, const float* uv)
{
	GL(Color4ubv((GLubyte*)&color));
	GL(TexCoord2fv(uv));
	GL(Vertex3fv(pos));
}

void DebugDrawGL::vertex(const float x, const float y, const float z, unsigned int color, const float u, const float v)
{
	GL(Color4ubv((GLubyte*)&color));
	GL(TexCoord2f(u, v));
	GL(Vertex3f(x, y, z));
}

void DebugDrawGL::end()
{
	GL(End());
	GL(LineWidth(1.0f));
	GL(PointSize(1.0f));
}

