// if the arrays in 2 different traces are permutations of each other
// the sum of all the values per array should be the same at the end for both traces

func main()
{
  const Int[] a;
  Int x;
  Int i;
  const Int k;
  const Int alength;

  i = 0;
  x = 0;
  while(i < alength)
  {
    x = x + a[i];
    i = i + 1;
  }
}


//TODO: wrong property, this property does not properly express the notion of a permutation
// counterexample: array1 = aab, array2 = abb.
(assert-not
  (=>
    (and
      (forall ((p1 Int))
        (exists ((p2 Int))
          (= (a l8 p1 t1) (a l8 p2 t2))
        )
      )
      (forall ((p1 Int))
        (exists ((p2 Int))
          (= (a l8 p1 t2) (a l8 p2 t1))
        )
      )
    )
    (= (x main_end t1) (x main_end t2))
  )
)
