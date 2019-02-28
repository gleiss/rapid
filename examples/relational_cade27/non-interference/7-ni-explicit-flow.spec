(two-traces)

func main()
{
  Int high1;
  Int high2;
  Int low1;
  const Int low2;

  high2 = low1;
  high1 = high2;

  while (high2 < low2) {
    high1 = high1 + 1;
  }

  low1 = high1;

}

(assert-not
  (=>
    (and
      (= (low1 l10 t1) (low1 l10 t2))
      (= (low2 t1) (low2 t2))
    )
    (= (low1 main_end t1) (low1 main_end t2))
  )
)
