
void __cdecl16far FUN_834b_04d3(int *param_1)

{
  int iVar1;
  int *piVar2;
  undefined2 uVar3;
  undefined2 unaff_SS;
  int local_a;
  int iStack_8;
  int local_6;
  int iStack_4;
  
  if (param_1 == (int *)0x0) {
    __F_SCOPY(0x2a88,0x4125,&local_a,unaff_SS);
  }
  else {
    uVar3 = (undefined2)((ulong)param_1 >> 0x10);
    piVar2 = (int *)param_1;
    local_a = *param_1 << 1;
    iStack_8 = piVar2[1] << 1;
    local_6 = piVar2[2] << 1;
    iStack_4 = piVar2[3] << 1;
  }
  FUN_22bf_07a5();
  iVar1 = FUN_54f6_08bf();
  if (iVar1 == 0) {
    (*DAT_451b_2a74)(0x54f6,0x6795,0x4125,CONCAT22(iStack_8,local_a),CONCAT22(iStack_4,local_6),
                     0x342a,0x4125,local_a + 400,iStack_8 + 0x1e);
    FUN_8961_0698();
  }
  else {
    uVar3 = 0x54f6;
    iVar1 = FUN_54f6_08c7();
    if ((param_1 == (int *)0x0) && (opt_hidden_map != 0)) {
      FUN_2012_0bb7(0x6795,0x4125,local_a,iStack_8,local_6,3);
      FUN_2012_0f08(0x6795,0x4125,local_a,iStack_8,iStack_4,3);
      uVar3 = 0x2012;
      FUN_2012_0bb7(0x6795,0x4125,local_a,iStack_8 + iStack_4 + -1,local_6,3);
    }
    (*DAT_451b_2a74)(uVar3,0x6795,0x4125,CONCAT22(iStack_8,local_a),CONCAT22(iStack_4,local_6),
                     0x342a,0x4125,iVar1 + local_a,iStack_8 + 0x3c);
  }
  FUN_22bf_0757();
  return;
}


