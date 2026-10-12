
int __cdecl16far stime(time_t *__when)

{
  undefined2 in_stack_00000006;
  undefined1 local_a [4];
  undefined1 local_6 [4];
  
  unixtodos((int)*___when,*(undefined2 *)((int)__when + 2),local_6);
  setdate(local_6);
  settime(local_a);
  return 0;
}


