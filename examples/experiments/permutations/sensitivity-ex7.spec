// for two arrays that only differ in one position, such that
// the absolute difference of the values at those positions is smaller than k
// we obtain 2 values for x (= sums over array vals per trace) s.t. the
// absolute difference is also smaller than k

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

// TODO: fix notion of difference up to 1 element
(conjecture
    (=>
        (exists ((o Int)(o2 Int))
          (forall ((p1_1 Int) (p1_2 Int))
            (exists ((p2_1 Int)(p2_2))
              (and
                (=> (not (= o p1_1) (=(a l8 p1_1 t1) (a l8 p2_1 t2)))
                (=> (not (= o2 p1_2) (=(a l8 p1_2 t2) (a l8 p2_2 t1)))
                (< (abs (- (a l8 o t1)(a l8 o2 t2))) k)
              )
            )
          )
        )
        (< (abs(- (x main_end t1) (x main_end t2)) ) k)
    )
  )
)
