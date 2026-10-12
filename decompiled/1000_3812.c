
void __cdecl16far
int86(undefined2 param_1,undefined2 param_2,undefined2 param_3,undefined2 param_4,undefined2 param_5
     )

{
  undefined1 local_a [8];
  
  segread(local_a);
  int86x(param_1,param_2,param_3,param_4,param_5,local_a);
  return;
}


