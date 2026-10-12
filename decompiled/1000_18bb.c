
void __cdecl16near __fpe_handler(void)

{
  code *pcVar1;
  int *in_BX;
  undefined2 unaff_SS;
  code *pcVar2;
  int local_8;
  int *local_6;
  
  _local_6 = (int *)CONCAT22(unaff_SS,in_BX);
  if (DAT_451b_31f8 != (code *)0x0 || DAT_451b_31fa != 0) {
    pcVar2 = (code *)(*DAT_451b_31f8)(0x1000,8,0,0);
    local_8 = (int)((ulong)pcVar2 >> 0x10);
    pcVar1 = (code *)pcVar2;
    (*DAT_451b_31f8)(0x1000,8,pcVar2);
    if ((local_8 == 0) && (pcVar1 == (code *)0x1)) {
      return;
    }
    if (pcVar1 != (code *)0x0 || local_8 != 0) {
      (*DAT_451b_31f8)(0x1000,8,0,0);
      (*pcVar1)(0x1000,8,*(undefined2 *)((int)(undefined8 *)&DAT_4125_391e + *_local_6 * 6 + 2));
      return;
    }
  }
  fprintf((FILE *)0x3b94,(char *)((int)(undefined2 *)&DAT_451b_01c4 + 1),
          (char *)s_Floating_point_error___s__4125_39a5,0x4125,
          *(undefined2 *)((int)(undefined8 *)&DAT_4125_391e + *_local_6 * 6 + 4),
          *(undefined2 *)((int)(undefined8 *)&DAT_4125_391e + *_local_6 * 6 + 6));
  abort();
  return;
}


