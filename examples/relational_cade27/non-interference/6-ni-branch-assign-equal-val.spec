(two-traces)

func main()
{
  const Int high;
  const Int low2;
  Int low1;

  if (high > 0) {
    low1 = low2 + low2;
  }
  else {
    low1 = 2 * low2;
  }

}

(conjecture
  (=>
    (and
      (= (low1 l9 t1) (low1 l9 t2))
      (= (low2 t1) (low2 t2))
    )
    (= (low1 main_end t1) (low1 main_end t2))
  )
)
