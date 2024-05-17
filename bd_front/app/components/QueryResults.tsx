import React from "react";

function extractAttributeStrings(data: any[]): string[] {
    const attributeStrings: string[] = [];

    Object.keys(data[0]).forEach((key) => {
        attributeStrings.push(key);
    });

    return attributeStrings;
}

type TableProps = {
    key: number;
    data: any;
    data_index: string[];
};

const Table = ({ key, data, data_index }: TableProps) => {
    return (
        <tbody>
            <tr className="bg-white border-b dark:bg-gray-800 dark:border-gray-700 hover:bg-gray-50 dark:hover:bg-gray-600">
                {data_index.map((attribute, index) => (
                    <td key={index} className="px-6 py-4">{data[attribute]}</td>
                ))}
            </tr>
            {/* <th
                    scope="row"
                    className="px-6 py-4 font-medium text-gray-900 whitespace-nowrap dark:text-white"
                >
                    {tablename}
                </th> */}
            {/* <td className="px-6 py-4">{numberofelements}</td> */}

        </tbody>

    );
};

const QueryResults = () => {
    const data = require("./dataquery.json");
    const data_index = extractAttributeStrings(require("./dataquery.json"));

    return (
        <div className="h-2/5 relative overflow-x-auto overflow-y-auto shadow-md sm:rounded-lg">
            <table className="w-full text-sm text-left rtl:text-right text-gray-500 dark:text-gray-400">
                <thead className="text-xs text-gray-700 uppercase bg-gray-50 dark:bg-gray-700 dark:text-gray-400">
                    <tr>
                        {data_index.map((attribute, index) => (
                            <th key={index} scope="col" className="px-6 py-3">
                                {attribute}
                            </th>
                        ))}
                    </tr>
                </thead>

                {data.map((data, index) => (
                    <Table
                        key={index}
                        data={data}
                        data_index={data_index}
                    />
                ))}
            </table>
        </div>
    );
};

export default QueryResults;
