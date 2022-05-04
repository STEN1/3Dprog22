#include "PathfindingNPC.h"

#include "VisualObject/Mesh.h"
#include "Core/Globals.h"
#include "Core/TextureManager.h"
#include "BlueTrophy.h"
#include "GameObject/Player.h"
#include "SolidWall.h"
#include <chrono>



class Graph
{
public:
	Graph(Scene& scene, const glm::vec3& center, const glm::vec3& extent)
		: m_Center{center}
		, m_Extent{extent}
		, m_Scene{scene}
	{
		// assume uniform size extent
		float minXZ = center.x - extent.x;
		float maxXZ = center.x + extent.x;
		int size = 32;
		float step = extent.x * 2.f / (float)size;
		// generate nodes
		m_Nodes.resize(size);
		for (int z = 0; z < m_Nodes.size(); z++)
		{
			m_Nodes[z].resize(size);
			for (int x = 0; x < m_Nodes[z].size(); x++)
			{
				m_Nodes[z][x] = new Node();
				m_Nodes[z][x]->aabb.extent = glm::vec3(step / 2.f);
				glm::vec3 pos{ 0.f };
				pos.x = minXZ + step * (float)x;
				pos.z = minXZ + step * (float)z;
				pos.y = 1.f + m_Scene.GetHeightFromHeightmap(pos);
				m_Nodes[z][x]->aabb.pos = pos;
			}
		}
		// generate edges
		for (int z = 0; z < m_Nodes.size(); z++)
		{
			for (int x = 0; x < m_Nodes[z].size(); x++)
			{
				for (int i = -1; i <= 1; i++)
				{
					for (int j = -1; j <= 1; j++)
					{
						if (i == 0 && j == 0)
							continue;
						if ((z + i) >= 0 && (z + i) < m_Nodes.size()
							&& (x + j) >= 0 && (x + j) < m_Nodes[z].size())
						{
							Edge* newEdge = new Edge();
							newEdge->From = m_Nodes[z][x];
							newEdge->To = m_Nodes[z + i][x + j];
							m_Nodes[z][x]->Edges.push_back(newEdge);
						}
					}
				}
			}
		}
		// remove blocked nodes and remove edge from neighbors
		auto walls = m_Scene.GetGameObjectsOfClass<SolidWall>();
		for (auto& row : m_Nodes)
		{
			for (auto it = row.begin(); it != row.end();)
			{
				auto node = (*it);
				bool itIsValid{ true };
				for (auto wall : walls)
				{
					if (node->aabb.Intersect(wall->GetPhysicsShape()))
					{
						// loop through node edges
						for (auto edge : node->Edges)
						{
							// find node in edge->To and erase.
							for (auto edgeIt = edge->To->Edges.begin();
								edgeIt != edge->To->Edges.end(); edgeIt++)
							{
								if ((*edgeIt)->To == node)
								{
									edge->To->Edges.erase(edgeIt);
									break;
								}

							}
						}

						it = row.erase(it);
						itIsValid = false;
						delete node;
						break;
					}
				}
				if (itIsValid)
					it++;
			}
		}
	}
	Path GetPath(const glm::vec3& start, const glm::vec3& end)
	{
		Node* startNode = GetClosestNode(start);
		Node* endNode = GetClosestNode(end);
		auto startTime = std::chrono::high_resolution_clock::now();
		m_LastPath = Dijkstra(startNode, endNode);
		auto endTime = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::milli> duration = endTime - startTime;
		LOG("Dijkstra calc time: " + std::to_string(duration.count()) + "ms");
		return m_LastPath;
	}
	void DebugDraw()
	{
		glm::vec4 outerColor{ 1.f, 1.f, 1.f, 1.f };
		glm::vec4 nodeColor{ 1.f, 0.f, 1.f, 1.f };
		glm::vec4 pathColor{ 0.f, 1.f, 0.f, 1.f };
		glm::vec4 edgeColor{ 0.f, 0.f, 1.f, 1.f };
		RenderWindow::Get()->DrawAABB(m_Center, m_Extent, outerColor);
		for (auto& row : m_Nodes)
		{
			for (auto node : row)
			{
				RenderWindow::Get()->DrawAABB(node->aabb.pos, node->aabb.extent, nodeColor);
				for (auto edge : node->Edges)
				{
					RenderWindow::Get()->DrawLine(edge->From->aabb.pos, edge->To->aabb.pos, pathColor);
				}
			}
		}
		for (uint32_t i = 0; i < m_LastPath.Edges.size() - 1; i++)
		{
			RenderWindow::Get()->DrawLine(m_LastPath.Edges[i].To->aabb.pos,
				m_LastPath.Edges[i + 1].To->aabb.pos, pathColor);
		}
	}
private:
	Node* GetClosestNode(const glm::vec3& pos)
	{
		//TODO: make this fast.
		float dist2{powf(5000.f, 2.f)};
		Node* closestNode{};
		for (auto& row : m_Nodes)
		{
			for (auto node : row)
			{
				float distPosToNode2 = glm::length2(node->aabb.pos - pos);
				if (distPosToNode2 < dist2)
				{
					closestNode = node;
					dist2 = distPosToNode2;
				}
			}
		}
		return closestNode;
	}
	Path Dijkstra(Node* start, Node* end)
	{
		std::priority_queue<Path, std::vector<Path>, std::greater<Path>> apq;
		std::vector<Node*> visited;
		// setup
		Edge startEdge;
		startEdge.To = start;
		Path startPath;
		startPath.Edges.push_back(startEdge);
		apq.push(startPath);
		Path shortestPath;
		while (!apq.empty()/* && !end->m_besokt*/)
		{
			Node* tempNode = apq.top().GetLastNode();
			if (tempNode == end)
			{
				shortestPath = apq.top(); // bare å returnere denne hær egentlig
				break;
			}
			Path tempPath = apq.top();
			apq.pop();
			for (auto& edge : tempNode->Edges)
			{
				if (!edge->To->Visited)
				{
					edge->To->Visited = true;
					visited.push_back(edge->To);
					Path newPath = tempPath;
					float dist = glm::length(edge->To->aabb.pos - tempNode->aabb.pos);
					newPath.Cost += dist;
					newPath.DistToEnd = glm::length(end->aabb.pos - edge->To->aabb.pos);
					newPath.Edges.push_back(*edge);
					apq.push(newPath);
				}
			}
		}

		for (auto node : visited)
			node->Visited = false;

		return shortestPath;
	}
	glm::vec3 m_Center;
	glm::vec3 m_Extent;
	Scene& m_Scene;
	Path m_LastPath;
	std::vector<std::vector<Node*>> m_Nodes;
};

PathfindingNPC::PathfindingNPC(Scene& scene, const glm::mat4& transform, const glm::vec3& pathfindingCenter, const glm::vec3& pathfindingExtents)
	: GameObject(scene, transform)
{
	m_Graph = std::make_unique<Graph>(m_scene, pathfindingCenter, pathfindingExtents);
	objectType = ObjectType::Simulated;
	m_vo = std::make_unique<Mesh>(*this,
		Globals::AssetPath + std::string("SecretObject.obj"),
		TextureManager::GetTexture("treehouse_door_color.png"));
	m_name = "PathfindingNPC";
	m_PathTargetTrophyPos = GetClosestTrophyPos();
	m_Path = m_Graph->GetPath(GetPosition(), GetClosestTrophyPos());
	UpdateTarget();
	glm::vec3 color{ 0.8f, 0.8f, 0.8f };
	m_scene.AddLight(this, color);
}

void PathfindingNPC::Update(float deltaTime)
{
	if (GameOver)
		return;
	if (m_Stunned)
	{
		m_StunTimer += deltaTime;
		if (m_StunTimer > 2.f)
			m_Stunned = false;
		return;
	}
	MoveToTarget(deltaTime);
}

void PathfindingNPC::DebugDraw()
{
	m_Graph->DebugDraw();
}

void PathfindingNPC::BeginOverlap(GameObject* other)
{
	if (other->GetName() == "Explosion" && m_Stunned == false)
	{
		m_Stunned = true;
		m_StunTimer = 0.f;
	}
	if (other->GetName() == "BlueTrophy")
	{
		LOG_HIGHLIGHT("ENEMY SCORE: " + std::to_string(++m_Score));
		if (m_Score == 10)
		{
			auto player = m_scene.GetGameObjectsOfClass<Player>();
			player[0]->Lose();
		}
	}
}

void PathfindingNPC::MoveToTarget(float deltaTime)
{
	glm::vec3 toTarget = m_Target - GetPosition();
	glm::vec3 toTrophy = m_PathTargetTrophyPos - GetPosition();
	float distanceToTarget = glm::length(toTarget);
	float distanceToTrophy = glm::length(toTrophy);
	if (distanceToTrophy < 0.5f)
	{
		UpdateTarget();
	}
	else if (distanceToTarget < 0.5f)
	{
		UpdateTarget();
	}
	if (m_Path.Edges.empty())
	{
		AddPositionOffset(glm::normalize(toTrophy) * deltaTime * m_Speed);
	}
	else
	{
		AddPositionOffset(glm::normalize(toTarget) * deltaTime * m_Speed);
	}
}

void PathfindingNPC::UpdateTarget()
{
	if (m_Path.Edges.empty())
	{
		UpdatePath();
		m_Path.Edges.erase(m_Path.Edges.begin());
	}
	if (!m_Path.Edges.empty())
	{
		m_Target = m_Path.Edges.front().To->aabb.pos;
		m_Path.Edges.erase(m_Path.Edges.begin());
	}
}

void PathfindingNPC::UpdatePath()
{
	m_PathTargetTrophyPos = GetClosestTrophyPos();
	m_Path = m_Graph->GetPath(GetPosition(), m_PathTargetTrophyPos);
}

glm::vec3 PathfindingNPC::GetClosestTrophyPos()
{
	auto trophies = m_scene.GetGameObjectsOfClass<BlueTrophy>();
	float dist2{ powf(5000.f, 2.f) };
	glm::vec3 out{ 0.f };
	for (auto trophy : trophies)
	{
		float distToTrophy2 = glm::length2(trophy->GetPosition() - GetPosition());
		if (distToTrophy2 < dist2)
		{
			dist2 = distToTrophy2;
			out = trophy->GetPosition();
		}
	}
	return out;
}