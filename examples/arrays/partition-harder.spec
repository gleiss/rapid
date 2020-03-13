// Harder versions of some of the properties from partition-example
// 1) each element in b is a copy of some element in a, which fulfils the range-bounds of a
// 2) each element in c is a copy of some element in a, which fulfils the range-bounds of a
// 3) for each element in a, which is greater or equal 0, there exists a copy of that element in b, which fulfils the range-bounds of b
// 4) for each element in a, which is smaller than 0, there exists a copy of that element in c, which fulfils the range-bounds of c
// 5) for each element in a, either there exists a copy of that element in b, which fulfils the range-bounds of b, or there exists a copy of that element in c, which fulfils the range-bounds of c

func main()
{
  const Int[] a;
  const Int alength;

  Int[] b;
  Int blength = 0;
  Int[] c;
  Int clength = 0;

	Int i = 0;
	while(i < alength)
	{
      if(a[i] >= 0)
      {
        b[blength] = a[i];
        blength = blength + 1;
      }
      else
      {
        c[clength] = a[i];
        clength = clength + 1;
      }
      i = i + 1;
	}
}

(axiom
  (forall ((it Nat))
    (<= 
      (blength (l19 it))
      (i (l19 it)) 
    )
  )
)
(axiom
  (forall ((it Nat))
    (<= 
      (clength (l19 it))
      (i (l19 it)) 
    )
  )
)

(conjecture
  (forall ((pos Int))
    (=>
      (and
        (<= 0 alength)
        (<= 0 pos)
        (< pos (blength main_end))
      )
      (exists ((pos2 Int))
        (and
          (<= 0 pos2)
          (< pos2 alength)
          (= (b main_end pos) (a pos2))
        )
      )
    )
  )
)

(conjecture
  (forall ((pos Int))
    (=>
      (and
        (<= 0 alength)
        (<= 0 pos)
        (< pos (clength main_end))
      )
      (exists ((pos2 Int))
        (and
          (<= 0 pos2)
          (< pos2 alength)
          (= (c main_end pos) (a pos2))
        )
      )
    )
  )
)

(conjecture
  (forall ((pos Int))
    (=>
      (and
        (<= 0 alength)
        (<= 0 pos)
        (< pos alength)
        (<= 0 (a pos))
      )
      (exists ((pos2 Int))
        (and
          (<= 0 pos2)
          (< pos2 (blength main_end))
          (= (b main_end pos2) (a pos))
        )
      )
    )
  )
)

(conjecture
  (forall ((pos Int))
    (=>
      (and
        (<= 0 alength)
        (<= 0 pos)
        (< pos alength)
        (< (a pos) 0)
      )
      (exists ((pos2 Int))
        (and
          (<= 0 pos2)
          (< pos2 (clength main_end))
          (= (c main_end pos2) (a pos))
        )
      )
    )
  )
)

(conjecture
  (forall ((pos Int))
    (=>
      (and
        (<= 0 alength)
        (<= 0 pos)
        (< pos alength)
      )
      (or
        (exists ((pos2 Int))
          (and
            (<= 0 pos2)
            (< pos2 (blength main_end))
            (= (b main_end pos2) (a pos))
          )
        )
        (exists ((pos2 Int))
          (and
            (<= 0 pos2)
            (< pos2 (clength main_end))
            (= (c main_end pos2) (a pos))
          )
        )
      )
    )
  )
)