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
primitive clock() = std_clock;
primitive setMain(f) = std_set_main;
primitive abort() = std_abort;

primitive serialize(x) = std_serialize;
primitive unserialize(x) = std_unserialize;

primitive callback = std_callback;
primitive fieldId(x) = std_field_id;
primitive field(o,f) = std_obj_field;
primitive fieldCall(o,f,params) = std_field_call;
