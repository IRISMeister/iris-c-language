int callin_routine_call();
int callin_function_call1();
int callin_function_call2();
int callin_routine_geterrorinfo();
int callin_classmethod_call1();
int callin_classmethod_call2();
int callin_globals_set_and_get();

#define RETURNIFERROR(rc) if (rc) {return -1;}