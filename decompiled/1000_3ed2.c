
long __cdecl16far atol(char *s)

{
  ulong uVar1;
  bool bVar2;
  uint uVar3;
  byte bVar4;
  uint uVar5;
  byte *pbVar6;
  undefined2 uVar7;
  
  uVar7 = (undefined2)((ulong)s >> 0x10);
  pbVar6 = (byte *)s;
  uVar3 = 0;
  uVar5 = 0;
  do {
    bVar4 = *pbVar6;
    pbVar6 = pbVar6 + 1;
  } while ((*(byte *)(bVar4 + 0x3a5f) & 1) != 0);
  bVar2 = false;
  if (bVar4 != 0x2b) {
    if (bVar4 != 0x2d) goto LAB_1000_3f03;
    bVar2 = true;
  }
  do {
    bVar4 = *pbVar6;
    pbVar6 = pbVar6 + 1;
LAB_1000_3f03:
    uVar1 = CONCAT22(uVar5,uVar3);
    if ((0x39 < bVar4) || (uVar1 = CONCAT22(uVar5,uVar3), bVar4 < 0x30)) goto LAB_1000_3f37;
    uVar1 = (ulong)uVar3;
    uVar5 = (uint)(uVar1 * 10);
    uVar3 = uVar5 + (byte)(bVar4 - 0x30);
    bVar4 = (char)(uVar1 * 10 >> 0x10) + CARRY2(uVar5,(uint)(byte)(bVar4 - 0x30));
    uVar5 = (uint)bVar4;
    uVar1 = (ulong)CONCAT12(bVar4,uVar3);
  } while (bVar4 == 0);
  while( true ) {
    bVar4 = *pbVar6;
    pbVar6 = pbVar6 + 1;
    if ((0x39 < bVar4) || (bVar4 < 0x30)) break;
    uVar1 = (uVar1 & 0xffff) * 10 + CONCAT22((int)(uVar1 >> 0x10) * 10,(uint)(byte)(bVar4 - 0x30));
  }
LAB_1000_3f37:
  if (bVar2) {
    uVar1 = CONCAT22(-(uint)((int)uVar1 != 0) - (int)(uVar1 >> 0x10),-(int)uVar1);
  }
  return uVar1;
}


