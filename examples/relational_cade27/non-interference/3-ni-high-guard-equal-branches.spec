(two-traces)

func main()
{
  const Int hi;
  Int lo;

  if(hi > 0)
  {
    lo = lo + 1;
  }
  else
  {
    lo = lo + 1;
  }
}

(assert-not
  (=>
    (= (lo l8 t1) (lo l8 t2))
    (= (lo main_end t1) (lo main_end t2))
  )
)
