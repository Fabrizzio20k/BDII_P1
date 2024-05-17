export default function SpaceQuery({ isSent, setSent, query, setQuery }: { isSent: boolean, setSent: (isSent: boolean) => void, query: string, setQuery: (query: string) => void }) {
    const handleSubmit = (event: any) => {
        event.preventDefault();
        setSent(true);
    };
    return (
        <div className="w-full flex flex-col items-center justify-center">
            <form onSubmit={handleSubmit} className="w-full">
                <div className="w-full flex flex-col shadow-md rounded my-4">
                    <textarea
                        className="shadow appearance-none border rounded w-full py-2 px-3 text-white leading-tight focus:outline-none focus:shadow-outline mb-4 resize-none"
                        rows={4}
                        placeholder="Escribe tu consulta SQL aquí..."
                        style={{ fontFamily: '"Fira Code", monospace' }}
                        value={query}
                        onChange={(e) => setQuery(e.target.value)}
                    />
                    <button className="bg-green-800 hover:bg-green-700 text-white font-bold py-2 px-4 rounded focus:outline-none focus:shadow-outline" type="submit">
                        Submit
                    </button>
                </div>
            </form>
        </div>
    );
}