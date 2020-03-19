(set-traces 2)

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
    i = i + 1;
    
    hi = hi + 1;
  }
}

(conjecture
   (=>
      (and
        (= (k t1) (k t2))
        (= (lo t1) (lo t2))
        (forall ((pos Int))
          (= (output (l7 zero) pos t1) (output (l7 zero) pos t2))
        )
      )
      (forall ((pos Int))
        (= (output main_end pos t1) (output main_end pos t2))
      )
   )
)
