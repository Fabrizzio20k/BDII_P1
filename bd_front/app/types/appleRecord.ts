export type AppleRecord = {
    id: number;
    track_name: string;
    size_bytes: number;
    currency: string;
    price: number;
    rating_count_tot: number;
    rating_count_ver: number;
    user_rating: number;
    user_rating_ver: number;
    ver: string;
    cont_rating: string;
    prime_genre: string;
    sup_devices_num: number;
    ipadSc_urls_num: number;
    lang_num: number;
    vpp_lic: number;
};

export type AppleRecordKeys = keyof AppleRecord;