
int __cdecl16far dos_read_huge(void *buf,long count,int handle)

{
  int iVar1;
  int iVar2;
  code *pcVar3;
  uint uVar4;
  uint uVar5;
  bool bVar6;
  
  while( true ) {
    bVar6 = false;
    uVar5 = 0x8000;
    if ((count._2_2_ == 0) && (bVar6 = 0x8000 < (uint)count, (uint)count < 0x8000)) {
      uVar5 = (uint)count;
    }
    pcVar3 = (code *)swi(0x21);
    uVar4 = (*pcVar3)();
    if ((bVar6) || (uVar5 != uVar4)) break;
    iVar1 = (uint)count - uVar5;
    iVar2 = count._2_2_ - (uint)((uint)count < uVar5);
    count = CONCAT22(iVar2,iVar1);
    if ((iVar1 == 0) && (iVar2 == 0)) {
      return 1;
    }
  }
  return 0;
}


