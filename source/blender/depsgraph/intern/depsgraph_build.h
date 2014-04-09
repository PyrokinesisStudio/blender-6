/*
 * ***** BEGIN GPL LICENSE BLOCK *****
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * The Original Code is Copyright (C) 2013 Blender Foundation.
 * All rights reserved.
 *
 * Original Author: Lukas Toenne
 * Contributor(s): None Yet
 *
 * ***** END GPL LICENSE BLOCK *****
 */

#ifndef __DEPSGRAPH_BUILD_H__
#define __DEPSGRAPH_BUILD_H__

#include "depsgraph_types.h"

#include "depsgraph_util_id.h"
#include "depsgraph_util_rna.h"
#include "depsgraph_util_string.h"

struct ListBase;
struct ID;
struct FCurve;
struct Group;
struct Main;
struct Object;
struct Scene;
struct World;

struct Depsgraph;
struct DepsNode;
struct RootDepsNode;
struct IDDepsNode;
struct TimeSourceDepsNode;
struct ComponentDepsNode;
struct OperationDepsNode;

struct DepsgraphNodeBuilder {
	DepsgraphNodeBuilder(Main *bmain, Depsgraph *graph);
	~DepsgraphNodeBuilder();
	
	RootDepsNode *add_root_node();
	IDDepsNode *add_id_node(IDPtr id);
	TimeSourceDepsNode *add_time_source(IDPtr id);
	ComponentDepsNode *add_component_node(IDDepsNode *id_node, eDepsNode_Type comp_type, const string &subdata = "");
	OperationDepsNode *add_operation_node(ComponentDepsNode *comp_node, eDepsNode_Type type,
	                                      eDepsOperation_Type optype, DepsEvalOperationCb op, const string &description,
	                                      PointerRNA ptr);
	
	IDDepsNode *build_scene(Scene *scene);
	SubgraphDepsNode *build_subgraph(Group *group);
	IDDepsNode *build_object(Object *ob);
	ComponentDepsNode *build_object_transform(Object *ob, IDDepsNode *ob_node);
	void build_constraints(ComponentDepsNode *comp_node, eDepsNode_Type constraint_op_type);
	void build_rigidbody(Scene *scene);
	void build_animdata(IDDepsNode *id_node);
	OperationDepsNode *build_driver(ComponentDepsNode *adt_node, FCurve *fcurve);
	void build_world(World *world);
	void build_compositor(Scene *scene);
	
private:
	Main *m_bmain;
	Depsgraph *m_graph;
};

struct RootKey
{
};

struct TimeSourceKey
{
	TimeSourceKey(IDPtr id = NULL) : id(id) {}
	IDPtr id;
};

struct IDKey
{
	IDKey(IDPtr id) : id(id) {}
	IDPtr id;
};

struct ComponentKey
{
	ComponentKey(IDPtr id, eDepsNode_Type type, const string &subdata = "") : id(id), type(type), subdata(subdata) {}
	IDPtr id;
	eDepsNode_Type type;
	string subdata;
};

struct OperationKey
{
	OperationKey(IDPtr id, eDepsNode_Type type, const string &name) : id(id), type(type), name(name) {}
	IDPtr id;
	eDepsNode_Type type;
	string name;
};

struct RNAPathKey
{
	RNAPathKey(IDPtr id, const string &path);
	RNAPathKey(IDPtr id, const PointerRNA &ptr, PropertyRNA *prop);
	IDPtr id;
	PointerRNA ptr;
	PropertyRNA *prop;
};

struct DepsgraphRelationBuilder {
	DepsgraphRelationBuilder(Depsgraph *graph);
	
	template <typename KeyFrom, typename KeyTo>
	void add_relation(const KeyFrom &key_from, const KeyTo &key_to,
	                  eDepsRelation_Type type, const string &description);
	
	void build_scene(Scene *scene);
	void build_object(Scene *scene, Object *ob);
	void build_object_parent(Object *ob);
	void build_constraints(Scene *scene, IDPtr id, eDepsNode_Type constraint_op_type, ListBase *constraints);
	void build_rigidbody(Scene *scene);
	void build_animdata(IDPtr id);
	void build_driver(IDPtr id, FCurve *fcurve);
	void build_world(Scene *scene, World *world);
	void build_compositor(Scene *scene);
	
protected:
	RootDepsNode *find_node(const RootKey &key) const;
	TimeSourceDepsNode *find_node(const TimeSourceKey &key) const;
	IDDepsNode *find_node(const IDKey &key) const;
	ComponentDepsNode *find_node(const ComponentKey &key) const;
	OperationDepsNode *find_node(const OperationKey &key) const;
	DepsNode *find_node(const RNAPathKey &key) const;
	
	void add_node_relation(DepsNode *node_from, DepsNode *node_to,
	                  eDepsRelation_Type type, const string &description);
	
private:
	Depsgraph *m_graph;
};

template <typename KeyFrom, typename KeyTo>
void DepsgraphRelationBuilder::add_relation(const KeyFrom &key_from, const KeyTo &key_to,
                                            eDepsRelation_Type type, const string &description)
{
	DepsNode *node_from = find_node(key_from);
	DepsNode *node_to = find_node(key_to);
	if (node_from && node_to) {
		add_node_relation(node_from, node_to, type, description);
	}
	else {
		if (!node_from) {
			/* XXX TODO handle as error or report if needed */
		}
		if (!node_to) {
			/* XXX TODO handle as error or report if needed */
		}
	}
}


#endif // __DEPSGRAPH_BUILD_H__
