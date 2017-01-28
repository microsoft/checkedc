// Unit tests for typechecking new Checked C function pointers
//
// The following line is for the LLVM test harness:
// RUN: %clang_cc1 -fcheckedc-extension -verify %s
//

#include "../../include/stdchecked.h"

int f0(int a) {
  return a;
}
float g0(float a) {
  return a;
}
int (*bad_f0)(int) = (int(*)(int))0xdeadbeef;

void takes_safe(_Ptr<int(int)> fun) {
  return;
}

// Nulls are specifically allowed
ptr<int(int)> allowed_convert_null0 = 0;
ptr<int(int)> allowed_convert_null1 = (int(*)(int))0;
ptr<int(int)> allowed_convert_null3 = *(int(*)(int))0;
ptr<int(int)> allowed_convert_null4 = &*(int(*)(int))0;
ptr<int(int)> allowed_convert_null6 = ***(int(*)(int))0;

// Named Functions are specifically allowed
ptr<int(int)> allowed_convert1 = f0;
// Pointers to named functions are specifically allowed
ptr<int(int)> allowed_convert2 = &f0;
ptr<int(int)> allowed_convert3 = *f0;
ptr<int(int)> allowed_convert4 = &*f0;
ptr<int(int)> allowed_convert5 = *&f0;
ptr<int(int)> allowed_convert6 = ***f0;

// Arbitrary Data is definitely not allowed
ptr<int(int)> bad_convert1 = (int(*)(int))0xdeadbeef; // expected-error {{cannot cast an unchecked function pointer to a checked pointer unless unchecked function is named}}
ptr<int(int)> bad_convert2 = (int(*)(int))g0; // expected-error {{cannot cast an unchecked function pointer to a checked pointer unless unchecked function is named}}

// Now locally within a function body
void local_convert(int(*f)(int)) {

  //
  // Casts at assignment-time
  //

  // Nulls are specifically allowed
  ptr<int(int)> safe_null0 = 0;
  ptr<int(int)> safe_null1 = (int(*)(int))0;
  ptr<int(int)> safe_null3 = *(int(*)(int))0;
  ptr<int(int)> safe_null4 = &*(int(*)(int))0;
  ptr<int(int)> safe_null6 = ***(int(*)(int))0;

  // Top-level declared functions are allowed
  ptr<int(int)> safe_f1 = f0;
  ptr<int(int)> safe_f2 = &f0;
  ptr<int(int)> safe_f3 = *f0;
  ptr<int(int)> safe_f4 = &*f0;
  ptr<int(int)> safe_f5 = *&f0;
  ptr<int(int)> safe_f6 = ***f0;

  // Parameter functions are not allowed
  ptr<int(int)> unsafe_f1 = f;   // expected-error {{cannot cast an unchecked function pointer to a checked pointer unless unchecked function is named}}
  ptr<int(int)> unsafe_f3 = *f;  // expected-error {{cannot cast an unchecked function pointer to a checked pointer unless unchecked function is named}}
  ptr<int(int)> unsafe_f4 = &*f; // expected-error {{cannot cast an unchecked function pointer to a checked pointer unless unchecked function is named}}
  ptr<int(int)> unsafe_f5 = *&f; // expected-error {{cannot cast an unchecked function pointer to a checked pointer unless unchecked function is named}}
  ptr<int(int)> unsafe_f6 = ***f; // expected-error {{cannot cast an unchecked function pointer to a checked pointer unless unchecked function is named}}

  // Arbitary data is definitely not allowed
  ptr<int(int)> bad_convert1 = (int(*)(int))0xdeadbeef; // expected-error {{cannot cast an unchecked function pointer to a checked pointer unless unchecked function is named}}
  ptr<int(int)> bad_convert2 = bad_f0; // expected-error {{cannot cast an unchecked function pointer to a checked pointer unless unchecked function is named}}
  ptr<int(int)> bad_convert3 = *bad_f0; // expected-error {{cannot cast an unchecked function pointer to a checked pointer unless unchecked function is named}}
  ptr<int(int)> bad_convert4 = ***bad_f0; // expected-error {{cannot cast an unchecked function pointer to a checked pointer unless unchecked function is named}}

  //
  // Casts in a Call
  //

  // This is allowed, from above, we'll use it further down to test checked
  // locals are ok.
  ptr<int(int)> local_safe = f0;

  // Nulls are specifically allowed
  takes_safe(0);
  takes_safe((int(*)(int))0);
  takes_safe(*(int(*)(int))0);
  takes_safe(&*(int(*)(int))0);
  takes_safe(***(int(*)(int))0);

  // Top-level declared functions are allowed
  takes_safe(f0);
  takes_safe(&f0);
  takes_safe(*f0);
  takes_safe(&*f0);
  takes_safe(*&f0);
  takes_safe(***f0);

  // Local checked functions are allowed
  takes_safe(local_safe);
  takes_safe(*local_safe);
  takes_safe(&*local_safe);
  takes_safe(*&local_safe);
  takes_safe(***local_safe);

  // Unchecked parameter functions are not allowed
  takes_safe(f);    // expected-error {{cannot cast an unchecked function pointer to a checked pointer unless unchecked function is named}}
  takes_safe(*f);   // expected-error {{cannot cast an unchecked function pointer to a checked pointer unless unchecked function is named}}
  takes_safe(&*f);  // expected-error {{cannot cast an unchecked function pointer to a checked pointer unless unchecked function is named}}
  takes_safe(*&f);  // expected-error {{cannot cast an unchecked function pointer to a checked pointer unless unchecked function is named}}
  takes_safe(***f); // expected-error {{cannot cast an unchecked function pointer to a checked pointer unless unchecked function is named}}

  // Arbitrary data is definitely not allowed
  takes_safe((int(*)(int))0xdeadbeef); // expected-error {{cannot cast an unchecked function pointer to a checked pointer unless unchecked function is named}}
  takes_safe(bad_f0);    // expected-error {{cannot cast an unchecked function pointer to a checked pointer unless unchecked function is named}}
  takes_safe(*bad_f0);   // expected-error {{cannot cast an unchecked function pointer to a checked pointer unless unchecked function is named}}
  takes_safe(***bad_f0); // expected-error {{cannot cast an unchecked function pointer to a checked pointer unless unchecked function is named}}

  //
  // Explicit User Casts
  //

  int(*local_unsafe)(int) = f0; // This is a valid unchecked to unchecked assignment

  ptr<int(int)> safe_cast_null0 = (ptr<int(int)>)0;
  ptr<int(int)> safe_cast_null1 = (ptr<int(int)>)((int(*)(int))0);
  ptr<int(int)> safe_cast_null3 = (ptr<int(int)>)(*(int(*)(int))0);
  ptr<int(int)> safe_cast_null4 = (ptr<int(int)>)(&*(int(*)(int))0);
  ptr<int(int)> safe_cast_null6 = (ptr<int(int)>)(***(int(*)(int))0);

  // Top-level declared functions are allowed
  ptr<int(int)> safe_cast_f1 =	(ptr<int(int)>)(f0);
  ptr<int(int)> safe_cast_f2 =	(ptr<int(int)>)(&f0);
  ptr<int(int)> safe_cast_f3 =	(ptr<int(int)>)(*f0);
  ptr<int(int)> safe_cast_f4 =	(ptr<int(int)>)(&*f0);
  ptr<int(int)> safe_cast_f5 =	(ptr<int(int)>)(*&f0);
  ptr<int(int)> safe_cast_f6 =	(ptr<int(int)>)(***f0);

  // Parameter functions are not allowed
  ptr<int(int)> unsafe_cast_f1 = (ptr<int(int)>)(f);   // expected-error {{cannot cast an unchecked function pointer to a checked pointer unless unchecked function is named}}
  ptr<int(int)> unsafe_cast_f3 = (ptr<int(int)>)(*f);  // expected-error {{cannot cast an unchecked function pointer to a checked pointer unless unchecked function is named}}
  ptr<int(int)> unsafe_cast_f4 = (ptr<int(int)>)(&*f); // expected-error {{cannot cast an unchecked function pointer to a checked pointer unless unchecked function is named}}
  ptr<int(int)> unsafe_cast_f5 = (ptr<int(int)>)(*&f); // expected-error {{cannot cast an unchecked function pointer to a checked pointer unless unchecked function is named}}
  ptr<int(int)> unsafe_cast_f6 = (ptr<int(int)>)(***f); // expected-error {{cannot cast an unchecked function pointer to a checked pointer unless unchecked function is named}}

  // Arbitary data is definitely not allowed
  ptr<int(int)> bad_cast_1 = (ptr<int(int)>)((int(*)(int))0xdeadbeef); // expected-error {{cannot cast an unchecked function pointer to a checked pointer unless unchecked function is named}}
  ptr<int(int)> bad_cast_2 = (ptr<int(int)>)(bad_f0); // expected-error {{cannot cast an unchecked function pointer to a checked pointer unless unchecked function is named}}
  ptr<int(int)> bad_cast_3 = (ptr<int(int)>)(*bad_f0); // expected-error {{cannot cast an unchecked function pointer to a checked pointer unless unchecked function is named}}
  ptr<int(int)> bad_cast_4 = (ptr<int(int)>)(***bad_f0); // expected-error {{cannot cast an unchecked function pointer to a checked pointer unless unchecked function is named}}

  //
  // Weird Casts
  //

  ptr<float(float, int)> local_safe2 = 0;
  int(*local_odd)(int) = local_unsafe;
  ptr<int(int)> local_super_unsafe1 = local_unsafe; // expected-error {{cannot cast an unchecked function pointer to a checked pointer unless unchecked function is named}}
  ptr<int(int)> local_super_unsafe2 = (ptr<int(int)>)local_unsafe; // expected-error {{cannot cast an unchecked function pointer to a checked pointer unless unchecked function is named}}

}