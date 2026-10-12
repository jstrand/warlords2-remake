
int __cdecl16far matherr(exception *__exc)

{
  undefined2 in_stack_00000006;
  
  if (___exc->type == 4) {
    *(undefined8 *)((int)&__exc->arg2 + 6) = 0;
  }
  else if (___exc->type != 5) {
    return 0;
  }
  return 1;
}


