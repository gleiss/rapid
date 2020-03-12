func main()
{
	const Int[] a;
	const Int[] b;
	Int[] c;

	const Int length;

	Int i = 0;
	Int clength = 0;
	while(i < length)
	{
		if (a[i] == b[i])
		{
			c[clength] = i;
			clength = clength + 1;
		}
		else
		{
			skip;
		}
		i = i + 1;
	}
}

(conjecture
	(forall ((pos Int))
		(=>
			(and
				(<= 0 pos)
				(< pos (clength main_end))
				(<= 0 length)
			)
			(=
				(a (c main_end pos))
				(b (c main_end pos))
			)
		)
	)
)

(axiom
	(forall ((it Nat))
		(<= (clength (l11 it)) (i (l11 it)))
	)
)

(conjecture
	(forall ((pos Int))
		(=>
			(and
				(<= 0 pos)
				(< pos (clength main_end))
				(<= 0 length)
			)
			(<= pos (c main_end pos))
		)
	)
)
