# PID Controller for Peristaltic Pump with Stepper Motor

Program untuk mengendalikan pompa peristaltik dengan menggunakan masukan laju aliran fluida yang diinginkan. Pompa tersebut digunakan untuk pengaliran fluida dalam aplikasi *microfluidics* (fluida berskala mikroliter).

Dirancang untuk sistem dengan mikrokontroler STM32F103C6 (*Blue Pill*), FT232RL USB-to-TTL (atau sejenisnya), dan sensor laju aliran fluida Sensirion SLF3S-1300F.

Data dapat direkam dengan menggunakan PuTTY atau aplikasi perekam data dari Serial Monitor yang lain dan disimpan dalam bentuk CSV.

Untuk membuat plot dari data yang telah diukur, jalankan file Plot.py dengan Python dengan perintah `python Plot.py file_csv`
(pastikan telah meng-*install* library yang diperlukan terlebih dahulu pada requirements.txt)
