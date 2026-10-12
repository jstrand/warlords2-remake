
void __cdecl16far FUN_7ecb_08e9(int *param_1,int *param_2)

{
  char *pcVar1;
  undefined2 uVar2;
  
  pcVar1 = *(char **)param_1;
  uVar2 = (undefined2)((ulong)pcVar1 >> 0x10);
  *param_2 = (*pcVar1 + -0x30) * 100 + (((char *)pcVar1)[1] + -0x30) * 10 + (int)((char *)pcVar1)[2]
             + -0x30;
  *param_1 = *param_1 + 3;
  return;
}


