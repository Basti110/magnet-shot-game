#pragma once

#include <string>
#include <glow/fwd.hh>

glow::SharedVertexArray load_mesh_from_obj(std::string const& filename, bool interpolate_tangents = true);
glow::SharedVertexArray load_mesh_from_ply(std::string const& filename);
