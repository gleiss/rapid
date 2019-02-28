// for each position p: if a[p] is 1, set b[p] to 1, otherwise set it to zero.
// Prove that either both a[p] is 1 and b[p] is 1, or both a[p] is not 1 and b[p] is zero
// similar to https://github.com/sosy-lab/sv-benchmarks/blob/master/c/array-examples/standard_running_true-unreach-call.c
// but simplified

func main()
{
  const Int[] a;
  const Int alength;
  Int[] b;

  Int i = 0;
  while(i < alength)
  {
    if (a[i] == 1)
    {
      b[i] = 1;
    }
    else
    {
      b[i] = 0;
    }
    i = i + 1;
  }
}


(assert-not
  (forall ((j Int))
    (=>
      (and
        (<= 0 j)
        (< j alength)
        (<= 0 alength)
      )
      (or
        (and
          (= (a j) 1)
          (= (b main_end j) 1)
        )
        (and
          (not (= (a j) 1) )
          (= (b main_end j) 0)
        )
      )
    )
  )
)
