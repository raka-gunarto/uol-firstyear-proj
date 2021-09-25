if [ -d "./database" ]; then
  cp -r "./database" "./database-backup"
fi
./build/tests/library_mgmt_sys_lib_tests