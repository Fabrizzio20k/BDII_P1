import random
import pandas as pd
from faker import Faker

fake = Faker()

# load all ids
ids = set()


def generate_version():
    # Generar una versión con formato de "10", "9.4.5", "7.23.2"
    major = random.randint(1, 99)
    if random.choice([True, False]):
        minor = random.randint(0, 99)
        if random.choice([True, False]):
            patch = random.randint(0, 99)
            return f"{major}.{minor}.{patch}"
        return f"{major}.{minor}"
    return f"{major}"


def generate_track_name(max_length):
    words = []
    while len(' '.join(words)) < max_length:
        word = fake.word()
        if len(' '.join(words + [word])) < max_length:
            words.append(word)
        else:
            break
    track_name = ' '.join(words)
    return track_name[:max_length]


def generate_data():
    # create an id that is not in the dataset
    id = random.randint(100000000, 999999999)
    while id in ids:
        id = random.randint(100000000, 999999999)
    data = {
        'id': id,
        'track_name': generate_track_name(50),
        'size_bytes': random.randint(1000, 9999999),
        'currency': random.choice(['USD', 'EUR', 'GBP']),
        'price': round(random.uniform(0, 100), 2),
        'rating_count_tot': random.randint(0, 1000000),
        'rating_count_ver': random.randint(0, 1000000),
        'user_rating': round(random.uniform(0, 5), 1),
        'user_rating_ver': round(random.uniform(0, 5), 1),
        'ver': generate_version(),
        'cont_rating': random.choice(['4+', '9+', '12+', '17+']),
        'prime_genre': random.choice(['Games', 'Social Networking', 'Photo & Video', 'Entertainment', 'Utilities', 'Weather', 'Health & Fitness', 'Productivity', 'Music', 'Lifestyle', 'News', 'Travel', 'Finance', 'Sports', 'Business', 'Education', 'Reference', 'Food & Drink', 'Navigation', 'Medical', 'Catalogs', 'Book']),
        'sup_devices.num': random.randint(1, 50),
        'ipadSc_urls.num': random.randint(0, 10),
        'lang.num': random.randint(1, 10),
        'vpp_lic': random.randint(0, 1),
    }
    ids.add(id)
    return data


new_data = []

for _ in range(1000):
    new_data.append(generate_data())

new_df = pd.DataFrame(new_data)
ids.clear()
new_data.clear()
new_df.to_csv('test_1000.csv', index=False)

for _ in range(10000):
    new_data.append(generate_data())

new_df = pd.DataFrame(new_data)
ids.clear()
new_data.clear()
new_df.to_csv('test_10000.csv', index=False)

for _ in range(100000):
    new_data.append(generate_data())

new_df = pd.DataFrame(new_data)
ids.clear()
new_data.clear()
new_df.to_csv('test_100000.csv', index=False)

for _ in range(1000000):
    new_data.append(generate_data())

new_df = pd.DataFrame(new_data)
ids.clear()
new_data.clear()
new_df.to_csv('test_1000000.csv', index=False)
