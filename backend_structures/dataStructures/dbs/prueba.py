import random
import pandas as pd

# Cargamos el dataset
df = pd.read_csv("AppleStore.csv", encoding='utf-8')

# Mostramos las primeras 5 filas
print(df.head())


# change the id field with random values of 5 numbers starting from 10000 and don't repeat
random_numbers = set()
while len(random_numbers) < df.shape[0]:
    random_numbers.add(random.randint(10000, 99999))

df['id'] = list(random_numbers)

# save the new dataset
df.to_csv("AppleStore_new.csv", index=False)
