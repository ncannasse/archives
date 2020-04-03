@ocamldoc -dot -dot-reduce -o archi.dot -I src -I src/disciple src/*.ml src/disciple/*.ml
@dot -Tpng archi.dot -o archi.png
@pause