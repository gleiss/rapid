func main()
{
	Int[] a;
	Int[] b;

	Int i;
	Int blength;

	i = 0;
	while(i < blength)
	{
		if(b[i] >= 0)
		{
			a[i] = b[i];
		}
		else
		{
			a[i] = -b[i];
		}
		i = i + 1;
	}
}

(assert-not
	(forall ((k Int))
		(=>
			(and
				(<= 0 k)
				(< k (blength main_end))
			)
			(or
				(= (a main_end k) (b main_end k) )
				(= (a main_end k) (- (b main_end k)) )
			)
		)
	)
)