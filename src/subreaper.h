#ifndef SUBREAPER_CLASS_H
#define SUBREAPER_CLASS_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/prctl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <thread>
#include <unistd.h>

#include "godot_cpp/variant/packed_string_array.hpp"
#include "godot_cpp/variant/string.hpp"
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <godot_cpp/core/binder_common.hpp>

class Subreaper : public godot::RefCounted {
  GDCLASS(Subreaper, godot::RefCounted);

protected:
  static void _bind_methods();

private:
public:
  // Constructor/deconstructor
  Subreaper();
  ~Subreaper();

  // Member functions
  static int create_process(godot::String path, godot::PackedStringArray args);
};

#endif // SUBREAPER_CLASS_H
