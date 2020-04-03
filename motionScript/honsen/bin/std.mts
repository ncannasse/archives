primitive class List = std_list;
primitive class Array = std_array;
primitive class String = std_string;
primitive class Hash = std_hash;
primitive class Random = std_random;
primitive class Xml = std_xml;

primitive int(x) = std_int;
primitive number(x) = std_number;
primitive string(x) = std_to_string;
primitive compare(x,y) = std_compare;

primitive print = std_print;
primitive random(x) = std_do_random;
primitive rnd() = std_random_float;
primitive clock() = std_clock;
primitive setMain(f) = std_set_main;
primitive abort() = std_abort;

primitive serialize(x) = std_serialize;
primitive unserialize(x) = std_unserialize;

primitive callback = std_callback;
primitive fieldId(x) = std_field_id;
primitive field(o,f) = std_obj_field;
primitive fieldCall(o,f,params) = std_field_call;

// Honsen Special

primitive closeLog() = honsen_close_log;

primitive class Vector = honsen_vector;
primitive class Matrix = honsen_matrix;
primitive class Trs = honsen_trs;
primitive class Quaternion = honsen_quat;
primitive class Camera = honsen_camera;
primitive class Light = honsen_light;

primitive setCamera(c) = honsen_set_camera;
primitive setLight(l) = honsen_set_light;

primitive vdot(v1,v2) = honsen_vector_dot;
primitive vcross(v1,v2) = honsen_vector_cross;
primitive vlerp(v1,v2,f) = honsen_vector_lerp;
primitive qslerp(q1,q2,f) = honsen_quaternion_slerp;
primitive vtransform(v,m) = honsen_vector_transform;

primitive class Clip = honsen_clip;
primitive class ClipPlayer = honsen_vect;
primitive class Mesh = honsen_mesh;
primitive class Texture = honsen_texture;

primitive class Key = honsen_key;
primitive getMouse() = honsen_mouse;

