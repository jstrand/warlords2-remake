
char * __cdecl16far string_lookup(int table,int group,int index)

{
  int sides;
  int iVar1;
  int iVar2;
  undefined2 in_DX;
  int *piVar3;
  undefined4 local_12;
  
  *(undefined1 *)(DAT_4125_267a * 0x50 + 0x59a8) = 0;
  iVar1 = mem_lock(*(undefined4 *)*(undefined4 *)(table * 4 + 0x267c));
  piVar3 = (int *)(iVar1 + group * 4);
  iVar2 = *piVar3;
  local_12 = (int *)CONCAT22(in_DX,piVar3 + 1);
  sides = *local_12;
  if (index < 0) {
    index = dice(1,sides,-1);
  }
  if (sides + -1 < index) {
    index = sides + -1;
  }
  strcpy((char *)CONCAT22(0x4125,(char *)(DAT_4125_267a * 0x50 + 0x59a8)),
         (char *)CONCAT22(in_DX,(char *)(iVar1 + *(int *)(iVar1 + iVar2 + index * 2))));
  mem_unlock(*(undefined4 *)*(undefined4 *)(table * 4 + 0x267c));
  iVar2 = DAT_4125_267a * 0x50;
  DAT_4125_267a = (DAT_4125_267a + 1) % 8;
  return (char *)CONCAT22(0x4125,(char *)(iVar2 + 0x59a8));
}


