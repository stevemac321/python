htmlname = '1689-chapter'
  
for num in range(26, 33):
	cmd = 'python to_html.py '
	hf = htmlname + str(num) + '.html '
	ch = 'ch' + str(num) + '.txt > results\\'
	print(cmd + hf + ch + hf)
	