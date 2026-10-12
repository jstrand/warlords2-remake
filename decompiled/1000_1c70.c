
void __exit(undefined2 param_1,int param_2,int param_3)

{
  undefined2 *puVar1;
  EVP_PKEY_CTX *unaff_SI;
  
  if (param_3 == 0) {
    while (DAT_4125_3a5c != 0) {
      DAT_4125_3a5c = DAT_4125_3a5c + -1;
      puVar1 = (undefined2 *)(DAT_4125_3a5c * 4 + 0x70d6);
      (*(code *)*puVar1)(0x1000);
    }
    _cleanup(unaff_SI);
    (*DAT_4125_3b60)(0x1000);
  }
  _restorezero();
  _checknull();
  if (param_2 == 0) {
    if (param_3 == 0) {
      (*DAT_4125_3b64)(0x1000);
      (*DAT_4125_3b68)(0x1000);
    }
    _terminate(param_1);
  }
  return;
}


