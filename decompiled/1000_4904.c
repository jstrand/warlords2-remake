
uint __strputn(int *param_1,uint param_2,void *param_3,undefined2 param_4)

{
  undefined2 uVar1;
  
  uVar1 = (undefined2)((ulong)param_1 >> 0x10);
  memcpy((void *)CONCAT22(((int *)param_1)[1],(void *)*param_1),(void *)CONCAT22(param_4,param_3),
         param_2);
  *param_1 = *param_1 + param_2;
  *(undefined1 *)*param_1 = 0;
  return param_2;
}


