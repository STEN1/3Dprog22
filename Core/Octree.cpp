#include "Octree.h"
#include "renderwindow.h"
#include "Core/Logger.h"
#include "GameObject/GameObject.h"

Octree::Octree(const AABB& physicsBox)
	: m_physicsBox(physicsBox)
{
}

Octree::~Octree()
{
	for (auto node : m_childNodes)
		delete node;
}

void Octree::DrawExtents()
{
	if (IsActiveLeaf())
	{
		glm::vec4 debugColor = { 1.f, 0.f, 0.f, 1.f };
		RenderWindow::Get()->DrawAABB(m_physicsBox.pos, m_physicsBox.extent, debugColor);
	}
	for (auto node : m_childNodes)
		node->DrawExtents();
}

void Octree::Insert(GameObject* gameObject)
{
	if (!gameObject->m_physicsShape->Intersect(&m_physicsBox))
		return;

	if (!IsLeaf())
	{	// not a leaf. send it down.
		for (auto node : m_childNodes)
			node->Insert(gameObject);
		return;
	}

	// is leaf and intersecting so insert
	m_gameObjects.emplace_back(gameObject);

	if (m_gameObjects.size() > m_maxObjects && m_physicsBox.extent.x > m_minExtent)
	{	// is leaf and is full. divide then send objects to children.
		Divide();
		for (auto node : m_childNodes)
			for (auto go : m_gameObjects)
				node->Insert(go);
		m_gameObjects.clear();
	}
}

void Octree::Divide()
{
	AABB a = m_physicsBox;
	a.extent /= 2.f;
	a.pos.x -= a.extent.x;
	a.pos.z -= a.extent.z;
	a.pos.y -= a.extent.y;
	AABB b = m_physicsBox;
	b.extent /= 2.f;
	b.pos.x -= b.extent.x;
	b.pos.z += b.extent.z;
	b.pos.y -= b.extent.y;
	AABB c = m_physicsBox;
	c.extent /= 2.f;
	c.pos.x += c.extent.x;
	c.pos.z += c.extent.z;
	c.pos.y -= c.extent.y;
	AABB d = m_physicsBox;
	d.extent /= 2.f;
	d.pos.x += d.extent.x;
	d.pos.z -= d.extent.z;
	d.pos.y -= d.extent.y;

	AABB e = m_physicsBox;
	e.extent /= 2.f;
	e.pos.x -= e.extent.x;
	e.pos.z -= e.extent.z;
	e.pos.y += e.extent.y;
	AABB f = m_physicsBox;
	f.extent /= 2.f;
	f.pos.x -= f.extent.x;
	f.pos.z += f.extent.z;
	f.pos.y += f.extent.y;
	AABB g = m_physicsBox;
	g.extent /= 2.f;
	g.pos.x += g.extent.x;
	g.pos.z += g.extent.z;
	g.pos.y += g.extent.y;
	AABB h = m_physicsBox;
	h.extent /= 2.f;
	h.pos.x += h.extent.x;
	h.pos.z -= h.extent.z;
	h.pos.y += h.extent.y;

	m_childNodes.reserve(8);
	m_childNodes.emplace_back(new Octree(a));
	m_childNodes.emplace_back(new Octree(b));
	m_childNodes.emplace_back(new Octree(c));
	m_childNodes.emplace_back(new Octree(d));
	m_childNodes.emplace_back(new Octree(e));
	m_childNodes.emplace_back(new Octree(f));
	m_childNodes.emplace_back(new Octree(g));
	m_childNodes.emplace_back(new Octree(h));
}

bool Octree::IsActiveLeaf()
{
	return (!m_gameObjects.empty() && IsLeaf());
}

void Octree::TryCollapse()
{
	// one less then max to make tree more stable under move calls.
	if (HasOnlyLeafs() && NumGameObjects() < m_maxObjects)
	{
		Collapse();
	}
}

void Octree::Collapse()
{
	for (auto node : m_childNodes)
	{
		for (auto go : node->m_gameObjects)
		{
			// need this so that there are no dupes in one node.
			// another solution would be to use std::set as 
			// a container for the gameobject pointers.
			// should maby test witch is faster. set would maby be slower for 
			// resolving overlaps.
			auto it = std::find(m_gameObjects.begin(), m_gameObjects.end(), go);
			if (it == m_gameObjects.end())
				m_gameObjects.emplace_back(go);
		}
		delete node;
	}
	m_childNodes.clear();
}

void Octree::Move(GameObject* gameObject, const glm::vec3& pos)
{
	Remove(gameObject);
	gameObject->m_transform[3].x = pos.x;
	gameObject->m_transform[3].y = pos.y;
	gameObject->m_transform[3].z = pos.z;
	gameObject->m_physicsShape->pos = pos;
	Insert(gameObject);
}

void Octree::Remove(GameObject* gameObject)
{
	if (!gameObject->m_physicsShape->Intersect(&m_physicsBox))
		return;
	if (IsLeaf())
	{
		auto it = std::find(m_gameObjects.begin(), m_gameObjects.end(), gameObject);
		if (it != m_gameObjects.end())
		{
			m_gameObjects.erase(it);
		}
	}
	else
	{
		for (auto node : m_childNodes)
			node->Remove(gameObject);
		TryCollapse();
	}
}

// Edge case where this wont remove end overlap with objects in neighboring oct that
// the gameobject just left. Causing a crash when calling EndOverlap on deleted object in resovecollisions()
// Potential fix 1: have gameobjects store last frame position, then move it through again? baaad
// Potential fix 2: simply itterate over all gameobjects to find gameobject that is deleted. Probably also bad.
// Potential fix 3: ??? look in to overlapping objects in go to find if some of them just ended!
// Fix in Scene::RemoveDestroyedGameObjects()
void Octree::RemoveAndEndOverlap(GameObject* gameObject)
{
	if (!gameObject->m_physicsShape->Intersect(&m_physicsBox))
		return;
	if (IsLeaf())
	{
		for (auto go : m_gameObjects)
		{
			auto it = std::find(go->m_overlappingGameObjects.begin(), go->m_overlappingGameObjects.end(), gameObject);
			if (it != go->m_overlappingGameObjects.end())
			{
				go->m_overlappingGameObjects.erase(it);
				go->EndOverlap(gameObject);
			}
		}

		auto it = std::find(m_gameObjects.begin(), m_gameObjects.end(), gameObject);
		if (it != m_gameObjects.end())
		{
			m_gameObjects.erase(it);
		}
	}
	else
	{
		for (auto node : m_childNodes)
			node->RemoveAndEndOverlap(gameObject);
		TryCollapse();
	}
}

void Octree::ResolveOverlappingGameObjects()
{
	if (!IsLeaf())
	{
		for (auto node : m_childNodes)
			node->ResolveOverlappingGameObjects();
		return;
	}
	for (uint32_t i = 0; i < m_gameObjects.size(); i++)
	{
		for (uint32_t j = i + 1; j < m_gameObjects.size(); j++)
		{
			bool intersecting = m_gameObjects[i]->m_physicsShape->Intersect(
				m_gameObjects[j]->m_physicsShape.get());
			// find j in i
			auto iIter = std::find(
				m_gameObjects[i]->m_overlappingGameObjects.begin(),
				m_gameObjects[i]->m_overlappingGameObjects.end(),
				m_gameObjects[j]);
			// intersecting and in array
			if (intersecting &&
				iIter != m_gameObjects[i]->m_overlappingGameObjects.end())
			{
				// do nothing
				continue;
			}
			// not intersecting and not in array
			if (!intersecting &&
				iIter == m_gameObjects[i]->m_overlappingGameObjects.end())
			{
				// do nothing
				continue;
			}
			// intersecting but not in array
			if (intersecting &&
				iIter == m_gameObjects[i]->m_overlappingGameObjects.end())
			{
				// push go to arrays
				m_gameObjects[i]->m_overlappingGameObjects.push_back(m_gameObjects[j]);
				m_gameObjects[j]->m_overlappingGameObjects.push_back(m_gameObjects[i]);
				// call events
				m_gameObjects[i]->BeginOverlap(m_gameObjects[j]);
				m_gameObjects[j]->BeginOverlap(m_gameObjects[i]);
				continue;
			}
			// not intersecting and in array
			if (!intersecting &&
				iIter != m_gameObjects[i]->m_overlappingGameObjects.end())
			{
				// find i in j
				auto jIter = std::find(
					m_gameObjects[j]->m_overlappingGameObjects.begin(),
					m_gameObjects[j]->m_overlappingGameObjects.end(),
					m_gameObjects[i]);
				// erase from arrays
				m_gameObjects[i]->m_overlappingGameObjects.erase(iIter);
				m_gameObjects[j]->m_overlappingGameObjects.erase(jIter);
				// call events
				m_gameObjects[i]->EndOverlap(m_gameObjects[j]);
				m_gameObjects[j]->EndOverlap(m_gameObjects[i]);
				continue;
			}
		}
	}
}

void Octree::Find(PhysicsShape* shape, std::vector<GameObject*>& outVector)
{
	if (!m_physicsBox.Intersect(shape))
		return;

	if (!IsLeaf())
	{
		for (auto node : m_childNodes)
			node->Find(shape, outVector);
		return;
	}

	for (auto go : m_gameObjects)
		if (shape->Intersect(go->m_physicsShape.get()))
			outVector.push_back(go);
}

void Octree::FindCollisions(std::vector<std::pair<GameObject*, GameObject*>>& outVector)
{

	if (!IsLeaf())
	{
		for (auto node : m_childNodes)
			node->FindCollisions(outVector);
		return;
	}

	for (uint32_t i = 0; i < m_gameObjects.size(); i++)
	{
		for (uint32_t j = i + 1; j < m_gameObjects.size(); j++)
		{
			if (m_gameObjects[i]->m_physicsShape->Intersect(m_gameObjects[j]->m_physicsShape.get()))
				outVector.emplace_back(m_gameObjects[i], m_gameObjects[j]);
		}
	}
}

bool Octree::IsLeaf()
{
	return m_childNodes.empty();
}

void Octree::FindActiveLeafs(std::vector<Octree*>& outVector)
{
	if (IsActiveLeaf())
		outVector.push_back(this);
	for (auto node : m_childNodes)
		node->FindActiveLeafs(outVector);
}

bool Octree::HasOnlyLeafs()
{
	for (auto node : m_childNodes)
		if (!node->IsLeaf())
			return false;
	return true;
}

uint32_t Octree::NumGameObjects()
{
	uint32_t numGameObjects = m_gameObjects.size();
	for (auto node : m_childNodes)
		numGameObjects += node->NumGameObjects();
	return numGameObjects;
}
