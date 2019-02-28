(two-traces)

func main()
{
  const Int k;
  const Int lo;
  Int hi = lo;
  Int i = 0;
  Int[] output;

  while(hi < k)
  {
    output[i] = hi;
    hi = hi + 1;
    i = i + 1;
  }
}

(assert-not
   (=>
      (and
        (= (k t1) (k t2))
        (= (lo t1) (lo t2))
        (forall ((pos Int))
          (= (output (l11 zero) pos t1) (output (l11 zero) pos t2))
        )
      )
      (forall ((pos Int))
        (= (output main_end pos t1) (output main_end pos t2))
      )
   )
)
