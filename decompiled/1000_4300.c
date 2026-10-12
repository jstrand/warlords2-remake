
void __cdecl16far
ultoa(undefined2 param_1,undefined2 param_2,undefined2 param_3,undefined2 param_4,undefined2 param_5
     )

{
  uint in_AX;
  
  __longtoa(0x61,in_AX & 0xff00,param_5,param_3,param_4,param_1,param_2);
  return;
}


