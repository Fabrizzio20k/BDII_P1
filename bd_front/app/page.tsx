"use client";

import { AppleRecord, AppleRecordKeys } from "./types/appleRecord";
import TableQuery from "./components/TableQuery/TableQuery";
import MessageDisplay from "./components/MessageDisplay/MessageDisplay";
import SpaceQuery from "./components/SpaceQuery/SpaceQuery";
import { useState, useEffect } from "react";

const headers: AppleRecordKeys[] = [
  "id",
  "track_name",
  "size_bytes",
  "currency",
  "price",
  "rating_count_tot",
  "rating_count_ver",
  "user_rating",
  "user_rating_ver",
  "ver",
  "cont_rating",
  "prime_genre",
  "sup_devices_num",
  "ipadSc_urls_num",
  "lang_num",
  "vpp_lic",
];

export default function Home() {

  const [created, isCreated] = useState(false);
  const [sent, isSent] = useState(false);
  const [query, setQuery] = useState('');

  const [data, setData] = useState<AppleRecord[]>([]);

  useEffect(() => {
    if (sent) {
      isCreated(true);
      isSent(false);
    }
  }, [sent]);

  return (
    <main className="h-screen w-auto flex flex-col justify-center items-center bg-gray-900 px-20">
      <SpaceQuery isSent={sent} setSent={isSent} query={query} setQuery={setQuery} />
      <div className="w-full flex justify-start items-start">
        <MessageDisplay message="No request made yet" status="error" />
      </div>
      <div>
        {/* <QueryResults /> */}
        {created && <TableQuery data={data} headers={headers} />}
        {!created && (
          <h2 className="text-white text-2xl font-bold my-36">
            Why not create the table first to see the data? 🤔
          </h2>
        )}

      </div >
    </main >);
}
