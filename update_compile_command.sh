
set +x

cd bin
cmake .. -DCMAKE_EXPORT_COMPILE_COMMANDS=1
cp compile_commands.json ..
