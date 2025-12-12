Relatório do trabalho da G2 - Hérica Becker

O trabalho foi criar um sistema com TinyML, focado na classificação de 3 estados de luminosidade, luz forte, luz média e sem luz, utilizando o Edge Impulse para treinar um modelo de classificação, executando no ESP32.

Sensores utilizados:

Sensor: sensor de luz bh1750

Atuador: led no D4, o led é ativado quando o modelo classifica que o sensor identificou a luz da lanterna.

Workflow TinyML: utilizei o Edge Impulse seguindo o passo a passo para criar um classificador de dados. Utilizei três classes para coletar os dados transmitidos pelo ESP32 no Data Aquisition: iluminacaonormal, lanterna, tapando e fiz 10 registros para cada tipo de classe. Após eu fui no create impulse para criar um impulso, configurei para adicionar os blocos de Spectral Analysis e Classification, salvei o impulso e fui para o Spectral features, cliquei em autotune parameters e depois no botão de save parameters, aí ele vai para Generate features e é gerado onde foram gerados os dados de variações e fui para o Classifier cliquei em save e train aí foram gerados os dados do treinamento com 69.4% de acurácia e erros 0.54. Depois em Configure your deployment, foi adicionada a biblioteca do arduino e carregado os dados que foram gerados no Edge via arquivo ZIP, então no Código do arduino modifiquei para funcionar e adicionei o arquivo ZIP dentro para utilizar no código. E depois testei tudo no serial monitor para ver o funcionamento.

Edge Impulse:
<img width="745" height="626" alt="edge" src="https://github.com/user-attachments/assets/b571ce29-c819-46eb-be4c-237cc913147c" />
<img width="730" height="637" alt="Edge2" src="https://github.com/user-attachments/assets/af2578bd-602c-4085-84d6-9dd58d48e52b" />



Resultado:

<img width="1512" height="712" alt="Captura de tela 2025-12-05 222700" src="https://github.com/user-attachments/assets/3e986f8a-b310-46ef-bbe2-8b55fc2269d4" />

<img width="1502" height="398" alt="Captura de tela 2025-12-06 113930" src="https://github.com/user-attachments/assets/5c24afc1-d2f6-4a62-b50b-fb29543692d6" />

<img width="896" height="395" alt="Captura de tela 2025-12-06 113958" src="https://github.com/user-attachments/assets/f5e53225-4081-4dc3-bf01-20d12e942823" />
