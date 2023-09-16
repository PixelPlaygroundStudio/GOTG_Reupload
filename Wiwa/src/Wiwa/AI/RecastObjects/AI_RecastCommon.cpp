#include <wipch.h>
#include "AI_RecastCommon.h"
#include "RecastDebugDraw.h"
#include "DetourDebugDraw.h"
#include "DetourNavMesh.h"
#include "DetourNavMeshQuery.h"
#include "DetourCrowd.h"
#include "../Geometry/InputGeom.h"

static const int NAVMESHSET_MAGIC = 'M' << 24 | 'S' << 16 | 'E' << 8 | 'T'; //'MSET';
static const int NAVMESHSET_VERSION = 1;

struct NavMeshSetHeader
{
	int magic;
	int version;
	int numTiles;
	dtNavMeshParams params;
};

struct NavMeshTileHeader
{
	dtTileRef tileRef;
	int dataSize;
};

dtNavMesh* RecastCommon::loadAll(const char* path)
{
	FILE* fp = fopen(path, "rb");
	if (!fp) return 0;

	// Read header.
	NavMeshSetHeader header;
	size_t readLen = fread(&header, sizeof(NavMeshSetHeader), 1, fp);
	if (readLen != 1)
	{
		fclose(fp);
		return 0;
	}
	if (header.magic != NAVMESHSET_MAGIC)
	{
		fclose(fp);
		return 0;
	}
	if (header.version != NAVMESHSET_VERSION)
	{
		fclose(fp);
		return 0;
	}

	dtNavMesh* mesh = dtAllocNavMesh();
	if (!mesh)
	{
		fclose(fp);
		return 0;
	}
	dtStatus status = mesh->init(&header.params);
	if (dtStatusFailed(status))
	{
		fclose(fp);
		return 0;
	}

	// Read tiles.
	for (int i = 0; i < header.numTiles; ++i)
	{
		NavMeshTileHeader tileHeader;
		readLen = fread(&tileHeader, sizeof(tileHeader), 1, fp);
		if (readLen != 1)
		{
			fclose(fp);
			return 0;
		}

		if (!tileHeader.tileRef || !tileHeader.dataSize)
			break;

		unsigned char* data = (unsigned char*)dtAlloc(tileHeader.dataSize, DT_ALLOC_PERM);
		if (!data) break;
		memset(data, 0, tileHeader.dataSize);
		readLen = fread(data, tileHeader.dataSize, 1, fp);
		if (readLen != 1)
		{
			dtFree(data);
			fclose(fp);
			return 0;
		}

		mesh->addTile(data, tileHeader.dataSize, DT_TILE_FREE_DATA, tileHeader.tileRef, 0);
	}

	fclose(fp);

	return mesh;
}
void RecastCommon::saveAll(const char* path, const dtNavMesh* mesh)
{
	if (!mesh) return;

	FILE* fp = fopen(path, "wb");
	if (!fp)
		return;

	// Store header.
	NavMeshSetHeader header;
	header.magic = NAVMESHSET_MAGIC;
	header.version = NAVMESHSET_VERSION;
	header.numTiles = 0;
	for (int i = 0; i < mesh->getMaxTiles(); ++i)
	{
		const dtMeshTile* tile = mesh->getTile(i);
		if (!tile || !tile->header || !tile->dataSize) continue;
		header.numTiles++;
	}
	memcpy(&header.params, mesh->getParams(), sizeof(dtNavMeshParams));
	fwrite(&header, sizeof(NavMeshSetHeader), 1, fp);

	// Store tiles.
	for (int i = 0; i < mesh->getMaxTiles(); ++i)
	{
		const dtMeshTile* tile = mesh->getTile(i);
		if (!tile || !tile->header || !tile->dataSize) continue;

		NavMeshTileHeader tileHeader;
		tileHeader.tileRef = mesh->getTileRef(tile);
		tileHeader.dataSize = tile->dataSize;
		fwrite(&tileHeader, sizeof(tileHeader), 1, fp);

		fwrite(tile->data, tile->dataSize, 1, fp);
	}

	fclose(fp);
}

RecastCommon::RecastCommon() :
	m_geom(0),
	m_navMesh(0),
	m_navQuery(0),
	m_Crowd(0),
	m_navMeshDrawFlags(DU_DRAWNAVMESH_OFFMESHCONS | DU_DRAWNAVMESH_CLOSEDLIST),
	m_filterLowHangingObstacles(true),
	m_filterLedgeSpans(true),
	m_filterWalkableLowHeightSpans(true),
	m_tool(0),
	m_ctx(0)
{
	resetCommonSettings();
	m_navQuery = dtAllocNavMeshQuery();
	m_Crowd = dtAllocCrowd();

	for (int i = 0; i < MAX_TOOLS; i++)
		m_toolStates[i] = 0;
}

RecastCommon::~RecastCommon()
{
	dtFreeNavMeshQuery(m_navQuery);
	dtFreeNavMesh(m_navMesh);
	dtFreeCrowd(m_Crowd);
	delete m_tool;
	for (int i = 0; i < MAX_TOOLS; i++)
		delete m_toolStates[i];
}

void RecastCommon::setTool(SampleTool* tool)
{
	delete m_tool;
	m_tool = tool;
	if (tool)
		m_tool->init(this);
}

void RecastCommon::handleSettings()
{
}

void RecastCommon::handleTools()
{
}

void RecastCommon::handleDebugMode()
{

}

void RecastCommon::handleClick(const float* s, const float* p, bool shift)
{
	if (m_tool)
		m_tool->handleClick(s, p, shift);
}

void RecastCommon::handleToggle()
{
	if (m_tool)
		m_tool->handleToggle();
}

void RecastCommon::handleStep()
{
	if (m_tool)
		m_tool->handleStep();
}

void RecastCommon::handleRender()
{
	if (!m_geom)
		return;

	// Draw mesh
	duDebugDrawTriMesh(&m_dd, m_geom->getMesh()->getVerts(), m_geom->getMesh()->getVertCount(),
		m_geom->getMesh()->getTris(), m_geom->getMesh()->getNormals(), m_geom->getMesh()->getTriCount(), 0, 1.0f);
	// Draw bounds
	const float* bmin = m_geom->getMeshBoundsMin();
	const float* bmax = m_geom->getMeshBoundsMax();
	duDebugDrawBoxWire(&m_dd, bmin[0], bmin[1], bmin[2], bmax[0], bmax[1], bmax[2], duRGBA(255, 255, 255, 128), 1.0f);
}

void RecastCommon::handleRenderOverlay(double* proj, double* model, int* view)
{
}

void RecastCommon::handleMeshChanged(InputGeom* geom)
{
	m_geom = geom;

	const BuildSettings* buildSettings = geom->getBuildSettings();
	if (buildSettings)
	{
		m_cellSize = buildSettings->cellSize;
		m_cellHeight = buildSettings->cellHeight;
		m_agentHeight = buildSettings->agentHeight;
		m_agentRadius = buildSettings->agentRadius;
		m_agentMaxClimb = buildSettings->agentMaxClimb;
		m_agentMaxSlope = buildSettings->agentMaxSlope;
		m_regionMinSize = buildSettings->regionMinSize;
		m_regionMergeSize = buildSettings->regionMergeSize;
		m_edgeMaxLen = buildSettings->edgeMaxLen;
		m_edgeMaxError = buildSettings->edgeMaxError;
		m_vertsPerPoly = buildSettings->vertsPerPoly;
		m_detailSampleDist = buildSettings->detailSampleDist;
		m_detailSampleMaxError = buildSettings->detailSampleMaxError;
		m_partitionType = buildSettings->partitionType;
	}
}

bool RecastCommon::handleBuild()
{
	return false;
}

void RecastCommon::handleUpdate(const float dt)
{
	if (m_tool)
		m_tool->handleUpdate(dt);
	updateToolStates(dt);
}

void RecastCommon::collectSettings(BuildSettings& settings)
{
	settings.cellSize = m_cellSize;
	settings.cellHeight = m_cellHeight;
	settings.agentHeight = m_agentHeight;
	settings.agentRadius = m_agentRadius;
	settings.agentMaxClimb = m_agentMaxClimb;
	settings.agentMaxSlope = m_agentMaxSlope;
	settings.regionMinSize = m_regionMinSize;
	settings.regionMergeSize = m_regionMergeSize;
	settings.edgeMaxLen = m_edgeMaxLen;
	settings.edgeMaxError = m_edgeMaxError;
	settings.vertsPerPoly = m_vertsPerPoly;
	settings.detailSampleDist = m_detailSampleDist;
	settings.detailSampleMaxError = m_detailSampleMaxError;
	settings.partitionType = m_partitionType;
}

bool RecastCommon::Save(const char* path)
{
	return false;
}

bool RecastCommon::Load(const char* path)
{
	return false;
}

void RecastCommon::updateToolStates(const float dt)
{
	for (int i = 0; i < MAX_TOOLS; i++)
	{
		if (m_toolStates[i])
			m_toolStates[i]->handleUpdate(dt);
	}
}

void RecastCommon::initToolStates(RecastCommon* sample)
{
	for (int i = 0; i < MAX_TOOLS; i++)
	{
		if (m_toolStates[i])
			m_toolStates[i]->init(sample);
	}
}

void RecastCommon::resetToolStates()
{
	for (int i = 0; i < MAX_TOOLS; i++)
	{
		if (m_toolStates[i])
			m_toolStates[i]->reset();
	}
}

void RecastCommon::renderToolStates()
{
	for (int i = 0; i < MAX_TOOLS; i++)
	{
		if (m_toolStates[i])
			m_toolStates[i]->handleRender();
	}
}

void RecastCommon::renderOverlayToolStates(double* proj, double* model, int* view)
{
	for (int i = 0; i < MAX_TOOLS; i++)
	{
		if (m_toolStates[i])
			m_toolStates[i]->handleRenderOverlay(proj, model, view);
	}
}


void RecastCommon::resetCommonSettings()
{
	m_cellSize = 0.3f;
	m_cellHeight = 0.2f;
	m_agentHeight = 2.0f;
	m_agentRadius = 0.6f;
	m_agentMaxClimb = 0.9f;
	m_agentMaxSlope = 45.0f;
	m_regionMinSize = 8;
	m_regionMergeSize = 20;
	m_edgeMaxLen = 12.0f;
	m_edgeMaxError = 1.3f;
	m_vertsPerPoly = 6.0f;
	m_detailSampleDist = 6.0f;
	m_detailSampleMaxError = 1.0f;
	m_partitionType = SAMPLE_PARTITION_WATERSHED;
}

void RecastCommon::handleCommonSettings()
{
	ImGui::Separator();
	float value01f = 0.1f;
	float value0 = 0.0f;
	float value1f = 1.0f;
	float value3f = 3.0f;
	float value5f = 5.0f;
	float value12f = 12.0f;
	float value16f = 16.0f;
	float value50f = 50.0f;
	float value90f = 90.0f;
	ImGui::Text("Rasterization");
	ImGui::SliderScalar("Cell Size", ImGuiDataType_Float, &m_cellSize, &value01f, &value1f, "%.2f");
	ImGui::SliderScalar("Cell Height", ImGuiDataType_Float, &m_cellHeight, &value01f, &value1f, "%.2f");
	if (m_geom)
	{
		const float* bmin = m_geom->getNavMeshBoundsMin();
		const float* bmax = m_geom->getNavMeshBoundsMax();
		int gw = 0, gh = 0;

		rcCalcGridSize(bmin, bmax, m_cellSize, &gw, &gh);
		char text[64];
		snprintf(text, 64, "Voxels  %d x %d", gw, gh);
		ImGui::Text(text);
	}
	ImGui::Separator();
	ImGui::Text("Agent");
	ImGui::SliderScalar("Height", ImGuiDataType_Float, &m_agentHeight, &value01f, &value5f, "%.2f");
	ImGui::SliderScalar("Radius", ImGuiDataType_Float, &m_agentRadius, &value0, &value5f, "%.2f");
	ImGui::SliderScalar("Max Climb", ImGuiDataType_Float, &m_agentMaxClimb, &value01f, &value5f, "%.2f");
	ImGui::SliderScalar("Max Slope", ImGuiDataType_Float, &m_agentMaxSlope, &value0, &value90f, "%.2f");

	ImGui::Separator();
	ImGui::Text("Partitioning");
	if (ImGui::RadioButton("Watershed", reinterpret_cast<int*>(&m_partitionType), SAMPLE_PARTITION_WATERSHED))
		m_partitionType = SAMPLE_PARTITION_WATERSHED;
	if (ImGui::RadioButton("Monotone", reinterpret_cast<int*>(&m_partitionType), SAMPLE_PARTITION_MONOTONE))
		m_partitionType = SAMPLE_PARTITION_MONOTONE;
	if (ImGui::RadioButton("Layers", reinterpret_cast<int*>(&m_partitionType), SAMPLE_PARTITION_LAYERS))
		m_partitionType = SAMPLE_PARTITION_LAYERS;

	ImGui::Separator();
	ImGui::Text("Polygonization");
	ImGui::SliderScalar("Max Edge Length", ImGuiDataType_Float, &m_edgeMaxLen, &value0, &value50f, "%.2f");
	ImGui::SliderScalar("Max Edge Error", ImGuiDataType_Float, &m_edgeMaxError, &value01f, &value3f, "%.2f");
	ImGui::SliderScalar("Verts Per Poly Climb", ImGuiDataType_Float, &m_vertsPerPoly, &value3f, &value12f, "%.2f");

	ImGui::Separator();
	ImGui::Text("Detail Mesh");
	ImGui::SliderScalar("Sample Distance", ImGuiDataType_Float, &m_detailSampleDist, &value0, &value16f, "%.2f");
	ImGui::SliderScalar("Max Sample Error", ImGuiDataType_Float, &m_detailSampleMaxError, &value0, &value16f, "%.2f");

	ImGui::Separator();
}

void RecastCommon::getCommonSettings(BuildSettings& settings)
{
	settings.cellSize = m_cellSize;
	settings.cellHeight = m_cellHeight;
	settings.agentHeight = m_agentHeight;
	settings.agentRadius = m_agentRadius;
	settings.agentMaxClimb = m_agentMaxClimb;
	settings.agentMaxSlope = m_agentMaxSlope;
	settings.regionMinSize = m_regionMinSize;
	settings.regionMergeSize = m_regionMergeSize;
	settings.edgeMaxLen = m_edgeMaxLen;
	settings.edgeMaxError = m_edgeMaxError;
	settings.vertsPerPoly = m_vertsPerPoly;
	settings.detailSampleDist = m_detailSampleDist;
	settings.detailSampleMaxError = m_detailSampleMaxError;
	settings.partitionType = m_partitionType;
}

SampleTool::~SampleTool()
{
}

SampleToolState::~SampleToolState()
{
}
