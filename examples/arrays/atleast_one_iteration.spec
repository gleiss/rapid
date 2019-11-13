func main()
{
  const Int[] a;
  const Int alength;

  Int i = 0;
  Int j = 0;
  while(i < alength)
  {
    i = i + 1;
    j = 1;
  }
}

(conjecture
  (=>
    (= 0 alength)
    (= (j main_end) 0)
  )
)

(conjecture
  (=>
    (< 0 alength)
    (= (j main_end) 1)
  )
)
